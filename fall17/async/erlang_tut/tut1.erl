-module(tut1).
-export([fac/1, mult/2, convert/2, convert_length/1, print_list/0]).%we export a list of comma separated funtion defined in this module.

fac(1)-> % function names MUST be in all small, a function is also called atom
	1; % ; tells that this function is not yet terminated and more to be written for this function definition to be complete
fac(N)-> %vaiables are conventionally capitals
	N * fac(N-1).
mult(X, Y)->
	X*Y.
convert(X, inch)-> % here 'inch' is an atom in erlang terms, not a variable, sort of a fixed name for this module
	X/2.54;	   % when this function convert is called it must be passed second arg as 'inch' or 'cm'
convert(X, cm)->
	X*2.54.

%tuples examples
convert_length({X, cm})-> % { } denotes a tuple; X is variable; cm is atom ; X and cm combined together are atom
	{X/2.54,inch};    % tuples can be nested like {stony,{wed,54}} to express max temp at stonybrook on wed to be 54
convert_length({X, inch})->
	{X*2.54,inch}.

%list examples
print_list()->
	[First, Second, Third] = [11,22,33]; % notice the capital F in First and likewise in the rest
	[Next | Rest] = [44, 45, 46, 47]. % a list can be split using |, here Next has 44 and Rest has [45,46,47]
