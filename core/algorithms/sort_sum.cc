
#include "properties/SortOrder.hh"
#include "algorithms/sort_sum.hh"

using namespace cadabra;

sort_sum::sort_sum(const Kernel& k, Ex& e)
	: Algorithm(k, e)
	{
	}

bool sort_sum::can_apply(iterator st)
	{
	if(*st->name=="\\sum") return true;
	else return false;
	}

Algorithm::result_t sort_sum::apply(iterator& st)
	{
	result_t ret=result_t::l_no_action;
	unsigned int num=tr.number_of_children(st);
	std::vector<sibling_iterator> sibs(num);
	sibling_iterator sib;
	sib = tr.begin(st);
	// Add all the sibling iterators
	for (unsigned int i=0; i < num; i++) {
		sibs[i] = sib;
		++sib;
		}
	// sort them
	std::stable_sort(sibs.begin(), sibs.end(), 
		[this](const sibling_iterator& sib1, const sibling_iterator& sib2) {
			int es=subtree_compare(&kernel.properties, sib1, sib2, -2, true, 0, true);
			return !should_swap(sib1, sib2, es);
			});

	// check if anything actually moved. any better way?
	sib=tr.begin(st);
	for (unsigned int i=0; i<num; i++) {
		if (sib != sibs[i]) {
			ret=result_t::l_applied;
			break;
			}
		++sib;
		}

	// rebuild the tree if something happened
	if (ret == result_t::l_applied) {
		st.node->first_child = sibs[0].node;
		st.node->last_child = sibs[num-1].node;
		for (unsigned int i = 0; i < num-1; i++) {
			sibs[i+1].node->prev_sibling = sibs[i].node;
			sibs[i].node->next_sibling = sibs[i+1].node;
			}
		sibs[0].node->prev_sibling = 0;
		sibs[num-1].node->next_sibling = 0;
		}

	return ret;

	}


bool sort_sum::should_swap(iterator obj1, iterator obj2, int subtree_comparison) const
	{
	sibling_iterator one=obj1, two=obj2;

	// Find a SortOrder property which contains both one and two.
	int num1, num2;
	const SortOrder *so1=kernel.properties.get<SortOrder>(one,num1);
	const SortOrder *so2=kernel.properties.get<SortOrder>(two,num2);

	if(so1==0 || so2==0) { // No sort order known
		if(subtree_comparison<0) return true;
		return false;
		}
	else if(abs(subtree_comparison)<=1) {   // Identical up to index names
		if(subtree_comparison==-1) return true;
		return false;
		}
	else {
		if(so1==so2) {
			if(num1>num2) return true;
			return false;
			}
		}

	return false;
	}


