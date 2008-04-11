--TEST--
Test posix_strerror() function : usage variations  - <type here specifics of this variation>
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "skip - POSIX extension not loaded"; 
?>
--FILE--
<?php
/* Prototype  : proto string posix_strerror(int errno)
 * Description: Retrieve the system error message associated with the given errno. 
 * Source code: ext/posix/posix.c
 * Alias to functions: 
 */

/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing posix_strerror() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

      // float data
      10.5,
      -10.5,
      10.1234567e10,
      10.7654321E-10,
      .5,

      // array data
      array(),
      array(0),
      array(1),
      array(1, 2),
      array('color' => 'red', 'item' => 'pen'),

      // null data
      NULL,
      null,

      // boolean data
      true,
      false,
      TRUE,
      FALSE,

      // empty data
      "",
      '',

      // string data
      "string",
      'string',

      // undefined data
      $undefined_var,

      // unset data
      $unset_var,
      
      // object data
      new stdclass(),
);

// loop through each element of the array for errno

foreach($values as $value) {
      echo "\nArg value $value \n";
      var_dump( posix_strerror($value) );
};

echo "Done";
?>
--EXPECTF--
*** Testing posix_strerror() : usage variations ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

Arg value 10.5 
string(18) "No child processes"

Arg value -10.5 
string(24) "Unknown error 4294967286"

Arg value 101234567000 
string(24) "Unknown error 2147483647"

Arg value 1.07654321E-9 
string(7) "Success"

Arg value 0.5 
string(7) "Success"

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Arg value Array 

Warning: posix_strerror() expects parameter 1 to be long, array given in %s on line %d
bool(false)

Arg value  
string(7) "Success"

Arg value  
string(7) "Success"

Arg value 1 
string(23) "Operation not permitted"

Arg value  
string(7) "Success"

Arg value 1 
string(23) "Operation not permitted"

Arg value  
string(7) "Success"

Arg value  

Warning: posix_strerror() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Arg value  

Warning: posix_strerror() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Arg value string 

Warning: posix_strerror() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Arg value string 

Warning: posix_strerror() expects parameter 1 to be long, string given in %s on line %d
bool(false)

Arg value  
string(7) "Success"

Arg value  
string(7) "Success"

Catchable fatal error: Object of class stdClass could not be converted to string in %s on line %d
