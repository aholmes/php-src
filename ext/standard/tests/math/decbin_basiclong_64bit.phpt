--TEST--
Test decbin function : 64bit long tests
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
 
define("MAX_64Bit", 9223372036854775807);
define("MAX_32Bit", 2147483647);
define("MIN_64Bit", -9223372036854775807 - 1);
define("MIN_32Bit", -2147483647 - 1);

$longVals = array(
    MAX_64Bit, MIN_64Bit, MAX_32Bit, MIN_32Bit, MAX_64Bit - MAX_32Bit, MIN_64Bit - MIN_32Bit,
    MAX_32Bit + 1, MIN_32Bit - 1, MAX_32Bit * 2, (MAX_32Bit * 2) + 1, (MAX_32Bit * 2) - 1,
    MAX_64Bit -1, MAX_64Bit + 1, MIN_64Bit + 1, MIN_64Bit - 1
);


foreach ($longVals as $longVal) {
   echo "--- testing: $longVal ---\n";
   var_dump(decbin($longVal));
}
   
?>
===DONE===
--EXPECT--
--- testing: 9223372036854775807 ---
unicode(63) "111111111111111111111111111111111111111111111111111111111111111"
--- testing: -9223372036854775808 ---
unicode(64) "1000000000000000000000000000000000000000000000000000000000000000"
--- testing: 2147483647 ---
unicode(31) "1111111111111111111111111111111"
--- testing: -2147483648 ---
unicode(64) "1111111111111111111111111111111110000000000000000000000000000000"
--- testing: 9223372034707292160 ---
unicode(63) "111111111111111111111111111111110000000000000000000000000000000"
--- testing: -9223372034707292160 ---
unicode(64) "1000000000000000000000000000000010000000000000000000000000000000"
--- testing: 2147483648 ---
unicode(32) "10000000000000000000000000000000"
--- testing: -2147483649 ---
unicode(64) "1111111111111111111111111111111101111111111111111111111111111111"
--- testing: 4294967294 ---
unicode(32) "11111111111111111111111111111110"
--- testing: 4294967295 ---
unicode(32) "11111111111111111111111111111111"
--- testing: 4294967293 ---
unicode(32) "11111111111111111111111111111101"
--- testing: 9223372036854775806 ---
unicode(63) "111111111111111111111111111111111111111111111111111111111111110"
--- testing: 9.2233720368548E+18 ---
unicode(64) "1000000000000000000000000000000000000000000000000000000000000000"
--- testing: -9223372036854775807 ---
unicode(64) "1000000000000000000000000000000000000000000000000000000000000001"
--- testing: -9.2233720368548E+18 ---
unicode(64) "1000000000000000000000000000000000000000000000000000000000000000"
===DONE===
