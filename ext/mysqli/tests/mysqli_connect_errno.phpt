--TEST--
mysqli_connect_errno()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	// too many parameter
	if (0 !== ($tmp = @mysqli_connect_errno($link)))
		printf("[001] Expecting integer/0, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (0 !== ($tmp = mysqli_connect_errno()))
		printf("[003] Expecting integer/0, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	$link = @mysqli_connect($host, $user . 'unknown_really', $passwd . 'non_empty', $db, $port, $socket);
	if (false !== $link)
		printf("[004] Connect to the server should fail using host=%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s, expecting boolean/false, got %s/%s\n",
			$host, $user . 'unknown_really', $db, $port, $socket, gettype($link), $link);

	if (0 === ($tmp = mysqli_connect_errno()))
		printf("[005] Expecting integer/any non-zero, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
done!