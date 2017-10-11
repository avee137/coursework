-module(tut). %name of the module, same as filename minus ".erl"
-export([double/1]). % telling that we are exporting a function named double hich takes on argument

double(X)-> % the function definition
	2*X. % . "fullstop" similar to ; in C

