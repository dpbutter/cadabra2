# on macOS
https://stackoverflow.com/questions/57535924/shipping-gtk-apps-for-macos-with-xcode


# 
# Test 1:= keep_terms

from cadabra import *;

obj = Ex('a*(b+c)')
distribute(obj,True)



How do we do this on the C++ side? 



-----

With a preprocessor we could turn

obj := A_{m n} B_{n p};

into 

obj = Ex('A_{m n} B_{n p}');

very easily and then all problems would go away. Ditto for

distribute!(obj);

to 

distribute(obj, all=True);

algo options:

 all:      like !  synonym for deep
 deep:     like !  make deep=True the default
   actually, we could make this algo dependent; for some algorithms
   like vary it does not make sense to do a deep default.
 repeat:   like !! make repeat=False the default

We could also wrap

distribute(%);

since (% is not a valid python sequence of characters anyway.


Properties:

  A_{m n}::Distributable(name='vector');

to
 
  Distributable('A_{m n}', name='vector');


sed -e 's/\(((\?!:=).)*\):=\(.*\)/\1 = Ex("\2")/'

---------------------

Indices('m,n,p,q,r', name='vector');
ex = 'A_{m n} B_{m n}';
substitute(ex, 'B_{p q} -> C_{p} C_{q}');




obj1 = Ex('a1 + a2 + a3 + a4 + a5 + a6 + a7');
obj1 = Algo(obj1, True);

obj1 = keep_terms(obj1, {2,4});  // auto-converts string to ex
// it is pass by reference, so obj1 gets modified. None of
// this silly obj1 = blabla(obj1) nonsense.
tst1 = 'a3 + a4 + a5 - obj1'; // python objects are known on the cdb side.
collect_terms(tst1);
assert(tst1);

ex = keep_terms('a1 + a2 + a3 + a4 + a5 + a6 + a7', {2,4});


@keep_terms(%){2}{4};
tst1:= a3 + a4 + a5 - @(obj1);
@collect_terms!(%);
@assert(tst1);


-----

a:=A*(B+C);
distribute(a, repeat=True);
b:=A_{m n} (B^{n p} + C^{n p});
distribute(b);
distribute('A_{m n} (B^{n p} + C^{n p})');

{a,b,c,d}::Indices;
ex:=A_{c d} C^{d};
rename_dummies(ex);

sort_product(ex)
ex:=B
ex:=A B A D C;
rl:= B -> Q;
substitute(ex, rl, True)

{m,n,p,a,b}::Indices.
ex:=A_{m n} ( C^{n p} + D^{n p} );
rl:= C^{a b} -> M_{m} D^{m a b};
substitute(ex, rl)

from cadabra import *
Indices(Ex('{m,n,p,a,b}'))
ex=Ex('A_{m n} ( C^{n p} + D^{n p} )')
rule=Ex('C^{a b} -> M_{m} D^{m a b}')
substitute(ex, rule, True)

substitute(ex, rl, True)


ex:= B + A B A D C + A A D C;
rl:= B -> 2;
substitute(ex, rl)
rl2:= A A D C -> 9
substitute(ex, rl2)
ex:= B + A B A D C + A A D C;
substitute(ex, rl)
rl:= B -> 2;
substitute(ex, rl2)

{m,n,p,a,b}::Indices.
ex:=A_{m n} ( C^{n p} + D^{n p} );
rl:= C^{a b} -> D^{a b};
substitute(ex, rl)


ex1=Ex(0)
ex2=Ex('0')
ex3=Ex('1')
print ex1==ex2
print ex2==ex3
print ex1==Ex(0)

def test():
	{m,n,p,a,b}::AntiCommuting.
	ex:=p m n a;
	sort_product(ex);
	print(ex);
	ex2:= A*(B+C);
	distribute(ex2);	
	tst2:= A B + A C - @(ex2)
	print(tst2)
	collect_terms(tst2)
        if tst==Ex(0):
       
	print(tree(tst2))
	print(tree(Ex(0))

test()

# This does not see the property declared in test():
ex3:=p m n a;
sort_product(ex3);
print(ex3);



a=3
ex:= A_{m n} @(a)
