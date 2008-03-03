--TEST--
Phar: tar-based phar corrupted
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--FILE--
<?php
include dirname(__FILE__) . '/files/make_invalid_tar.php.inc';

$tar = new corrupter(dirname(__FILE__) . '/tar_001.phar.tar', 'none');
$tar->init();
$tar->addFile('tar_001.phpt', __FILE__);
$tar->close();

$tar = fopen('phar://' . dirname(__FILE__) . '/tar_001.phar.tar/tar_001.phpt', 'rb');
try {
	$phar = new Phar(dirname(__FILE__) . '/tar_001.phar.tar');
	echo "should not execute\n";
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_001.phar.tar');
?>
--EXPECTF--
Warning: fopen(phar://C:\sandbox\pecl\phar\tests\tar/tar_001.phar.tar/tar_001.phpt): failed to open stream: phar error: "C:\sandbox\pecl\phar\tests\tar\tar_001.phar.tar" is a corrupted tar file in C:\sandbox\pecl\phar\tests\tar\tar_001.php on line 9
phar error: "C:\sandbox\pecl\phar\tests\tar\tar_001.phar.tar" is a corrupted tar file
===DONE===
