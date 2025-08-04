#include "NTensor.hh"
#include <stdexcept>
#include "Stopwatch.hh"
#include <cassert>
#include <cmath>
#include <string>
#ifdef HAS_TBB
#include <execution>
#endif
#include <algorithm>

using namespace cadabra;

// #define DEBUG 1

#ifdef DEBUG
#warning "DEBUG enabled for NTensor.cc"
static bool debug_stop = false;
#define DEBUGLN(ln) if(!debug_stop) { ln; }
#else
#define DEBUGLN(ln)
#endif

NTensor::NTensor(const std::vector<size_t>& shape_, std::complex<double> val)
	: shape(shape_)
	{
	size_t len=1;
	for(auto dim: shape)
		len *= dim;

	values.resize(len);
	for(auto& v: values)
		v=val;
	}

NTensor::NTensor(const std::vector<std::complex<double>>& vals)
	: values(vals)
	{
	shape.push_back(values.size());
	}

NTensor::NTensor(const std::vector<double>& vals)
	{
	for(size_t i=0; i<vals.size(); ++i)
		values.push_back(vals[i]);
	shape.push_back(values.size());
	}

NTensor::NTensor(std::initializer_list<std::complex<double>> vals)
	: NTensor(std::vector<std::complex<double>>(vals.begin(), vals.end()))
	{
	}

NTensor::NTensor(std::initializer_list<double> vals)
	: NTensor(std::vector<double>(vals.begin(), vals.end()))
	{
	}

NTensor::NTensor(std::complex<double> val)
	{
	values.push_back(val);
	shape.push_back(1);
	}

NTensor::NTensor(double val)
	{
	values.push_back(std::complex<double>(val, 0));
	shape.push_back(1);
	}

NTensor::NTensor(const NTensor& other)
	{
//	std::cerr << "copy-constructor " << other.values.size() << std::endl;
	DEBUGLN( std::cerr << "NTensor(const NTensor&): " << other.shape.size()
				<< ", " << other.values.size() << std::endl; );
	shape=other.shape;
	values=other.values;
	}

NTensor::NTensor(NTensor&& other)
	: shape(std::move(other.shape))
	, values(std::move(other.values))
	{
	}

NTensor& NTensor::operator=(NTensor&& other) noexcept
	{
	if(this != &other) {
		shape = std::move(other.shape);
		values = std::move(other.values);
		}
	return *this;
	}

NTensor& NTensor::operator=(const NTensor&& other) noexcept
	{
	if(this != &other) {
		shape = std::move(other.shape);
		values = std::move(other.values);
		}
	return *this;
	}

NTensor NTensor::linspace(std::complex<double> from, std::complex<double> to, size_t steps)
	{
	NTensor res(std::vector<size_t>({steps}), 0.0);
	if(steps <= 1)
		throw std::range_error("NTensor::linspace: steps must be greater than 1");

	for(size_t i=0; i<steps; ++i) {
		res.values[i] = from + (double)(i)*(to-from)/(double(steps-1));
		}
	return res;
	}

NTensor& NTensor::operator=(const NTensor& other)
	{
	shape=other.shape;
	values=other.values;
	return *this;
	}

std::complex<double> NTensor::at() const
	{
//	if(shape.size()!=1 || shape[0]!=1)
//		throw std::range_error("NTensor::at: cannot convert tensor "+std::to_string(shape.size())
//									  +", "+std::to_string(shape[0])+" to single scalar.");

	if(values.size()!=1)
		throw std::range_error("NTensor::at: inconsistent value array.");
	
	return values[0];
	}

std::complex<double> NTensor::at(const std::vector<size_t>& indices) const
	{
	if(indices.size()!=shape.size())
		throw std::range_error("NTensor::at: number of indices != shape length.");

	size_t idx = 0;
	size_t stride=1;

	for(size_t p=indices.size(); p-- != 0; ) {
		if(indices[p]>=shape[p])
			throw std::range_error("NTensor::at: index out of range.");
		idx += stride*indices[p];
		stride *= shape[p];
		}

	if(idx >= values.size())
		throw std::range_error("NTensor::at: indices out of range.");

	return values[idx];
	}

std::complex<double>& NTensor::at(const std::vector<size_t>& indices)
	{
	if(indices.size()!=shape.size())
		throw std::range_error("NTensor::at: number of indices != shape length.");

	size_t idx = 0;
	size_t stride=1;

	for(size_t p=indices.size(); p-- != 0; ) {
		if(indices[p]>=shape[p])
			throw std::range_error("NTensor::at: index out of range.");
		idx += stride*indices[p];
		stride *= shape[p];
		}

	if(idx >= values.size())
		throw std::range_error("NTensor::at: indices out of range.");

	return values[idx];
	}

std::ostream& cadabra::operator<<(std::ostream &str, const NTensor &nt)
	{
	// For an {a,b} tensor, we display as a vector of size 'a', each
	// element of which is a vector of size 'b'. And so on.

	bool isreal = nt.is_real();

	for(size_t p=0; p<nt.shape.size(); ++p)
		str << "[";

	for(size_t i=0; i<nt.values.size(); ++i) {
		if(isreal) str << nt.values[i].real();
		else       {
			str << nt.values[i].real();
			if(nt.values[i].imag()<0) str << " - ";
			else                      str << " + ";
			str << std::abs(nt.values[i].imag()) << "i";
			}

		// Closing/re-opening.
		size_t mult=1;
		for(size_t p=nt.shape.size(); p-- != 0; ) {
			// 2,4,3 -> 2, 8, 24
			mult *= nt.shape[p];
			if((i+1)%mult == 0)
				str << "]";
			}

		if(i+1<nt.values.size()) {
			str << ", ";
			mult=1;
			for(size_t p=nt.shape.size(); p-- != 0; ) {
				mult *= nt.shape[p];
				if((i+1)%mult == 0)
					str << "[";
				}
			}
		}

	return str;
	}

NTensor& NTensor::apply(std::complex<double> (*fun)(const std::complex<double>&))
	{
#ifdef HAS_TBB
	std::transform(std::execution::par_unseq,
						values.begin(), values.end(),
						values.begin(),
						fun);
#else
	for(auto& v: values)
		v = fun(v);
#endif

	return *this;
	}

NTensor& NTensor::operator+=(const NTensor& other)
	{
	if(other.shape.size()==1 && other.shape[0]==1) {
		// Arbitrary size base plus a scalar; add it to all.
		for(size_t i=0; i<values.size(); ++i)
			values[i] += other.values[0];
		}
	else if(shape.size()==1 && shape[0]==1) {
		// Scalar plus arbitrary size other.
		auto add = values[0];
		values = other.values;
		for(size_t i=0; i<values.size(); ++i)
			values[i] += add;
		shape=other.shape;
		}
	else {
		if(shape.size() != other.shape.size())
			throw std::range_error("NTensor::operator+=: shape lengths do not match, "+std::to_string(shape.size())
										  + " versus " + std::to_string(other.shape.size()) + ".");
		
		for(size_t p=0; p<shape.size(); ++p)
			if(shape[p]!=other.shape[p])
				throw std::range_error("NTensor::operator+=: shapes do not match.");
		
		for(size_t i=0; i<values.size(); ++i)
			values[i] += other.values[i];
		}
	
	return *this;
	}

NTensor& NTensor::operator*=(const NTensor& other)
	{
	if(other.shape.size()==1 && other.shape[0]==1) {
		// Arbitrary size base times a scalar.
		for(size_t i=0; i<values.size(); ++i)
			values[i] *= other.values[0];
		}
	else if(shape.size()==1 && shape[0]==1) {
		// Scalar times arbitrary size other.
		auto mult = values[0];
		values = other.values;
		for(size_t i=0; i<values.size(); ++i)
			values[i] *= mult;
		shape=other.shape;
		}
	else {
		if(shape.size() != other.shape.size()) {
			DEBUGLN( std::cerr << *this << "\n" << other << std::endl; )
			throw std::range_error("NTensor::operator*=: shape lengths do not match, "+std::to_string(shape.size())
										  + " versus " + std::to_string(other.shape.size()) + ".");
			}
		
		for(size_t p=0; p<shape.size(); ++p)
			if(shape[p]!=other.shape[p])
				throw std::range_error("NTensor::operator*=: shapes do not match.");
		
		for(size_t i=0; i<values.size(); ++i)
			values[i] *= other.values[i];
		}

	return *this;
	}

NTensor& NTensor::operator*=(const std::complex<double>& m)
	{
	for(size_t i=0; i<values.size(); ++i)
		values[i] *= m;
	return *this;
	}

NTensor& NTensor::operator*=(double m)
	{
	for(size_t i=0; i<values.size(); ++i)
		values[i] *= m;
	return *this;
	}

NTensor& NTensor::pow(const NTensor& other)
	{
	if(other.shape.size()==1 && other.shape[0]==1) {
		// Arbitrary size base to the power of a scalar.
		for(size_t i=0; i<values.size(); ++i)
			values[i] = std::pow(values[i], other.values[0]);
		}
	else {
		if(shape.size() != other.shape.size())
			throw std::range_error("NTensor::pow: shape lengths do not match, "+std::to_string(shape.size())
										  + " versus " + std::to_string(other.shape.size()) + ".");

		for(size_t p=0; p<shape.size(); ++p)
			if(shape[p]!=other.shape[p])
				throw std::range_error("NTensor::pow: shapes do not match; direction "
											  +std::to_string(p)+": "
											  +std::to_string(shape[p])+" vs. "
											  +std::to_string(other.shape[p])+".");
		
		for(size_t i=0; i<values.size(); ++i)
			values[i] = std::pow(values[i], other.values[i]);
		}
	
	return *this;
	}

NTensor NTensor::outer_product(const NTensor& a, const NTensor& b)
	{
	// std::cerr << "multiplying " << a << "\n"
	// 			 << "       with " << b << std::endl;
	std::vector<size_t> res_shape;
	res_shape.insert(res_shape.end(), a.shape.begin(), a.shape.end());
	res_shape.insert(res_shape.end(), b.shape.begin(), b.shape.end());

	NTensor res( res_shape, 0.0 );

	for(size_t i=0; i<res.values.size(); ++i) {
		size_t idx_a = i / b.values.size();
		size_t idx_b = i % b.values.size();
		if(idx_a >= a.values.size())
			throw std::range_error("NTensor::outer_product: index a out of range");
		if(idx_b >= b.values.size())
			throw std::range_error("NTensor::outer_product: index b out of range");

		res.values[i] = a.values[idx_a] * b.values[idx_b];
		}

	return res;
	}

bool NTensor::is_real() const
	{
	for(const auto& value: values)
		if(std::abs(value.imag()) > std::numeric_limits<double>::epsilon())
			return false;
	return true;
	}

bool NTensor::is_scalar() const
	{
	return (shape.size()==1 && shape[0]==1);
	}

NTensor NTensor::broadcast(std::vector<size_t> new_shape, size_t pos) const
	{
	// for(auto s: new_shape)
	// 	std::cerr << s << ", ";
	// std::cerr << "\n" << pos << std::endl;
	if(pos >= new_shape.size())
		throw std::range_error("NTensor::broadcast: pos out of range");
	if(shape.size() != 1)
		throw std::range_error("NTensor::broadcast: tensor must be rank-1");
	if(new_shape[pos] != shape[0])
		throw std::range_error("NTensor::broadcast: shape mismatch");


	NTensor res(new_shape, 0.);

	size_t lower = 1, higher=1;
	for(size_t s=pos+1; s<new_shape.size(); ++s)
		lower  *= new_shape[s];
	higher = lower * new_shape[pos];

	//  std::cerr << "lower: " << lower << "\nhigher: " << higher << std::endl;

	// Stopwatch sw;
	// sw.start();
	for(size_t i=0; i<res.values.size(); ++i) {
		size_t orig_i = (i % higher) / lower;
		// std::cerr << i << " -> " << orig_i << std::endl;
		if(orig_i >= new_shape[pos])
			throw std::range_error("NTensor::broadcast: computed index out of range");

		res.values[i] = values[orig_i];
		}

	// sw.stop();
	// std::cerr << "broadcast to " << res.values.size() <<  " took " << sw << std::endl;

	return res;
	}
