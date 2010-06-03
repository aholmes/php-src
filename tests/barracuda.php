<?php

$pcntl = true;
$ncurses = true;
$memcache = true;

$fail = false;

/**************************
 *           pcntl        *
 **************************/
if ($pcntl){
	echo "
/**************************
 *           pcntl        *
 **************************/
";

	if (!function_exists('pcntl_signal_dispatch'))
	{
		echo "FAIL\npcntl_signal_dispatch does not exist";
		$fail = true;
	}

	if (!$fail)
	{
		echo "Installing signal handler...\n";
		pcntl_signal(SIGHUP, 'sig');


		echo "Generating signal SIGHUP to self...\n";
		posix_kill(posix_getpid(), SIGHUP);

		echo "pcntl_signal_dispatch exists, calling it...\n";
		pcntl_signal_dispatch();
	}

	$fail = false;
	sleep(3);
}

/**************************
 *         ncurses        *
 **************************/
if ($ncurses)
{
	echo "
/**************************
 *         ncurses        *
 **************************/
";

	if (!function_exists('ncurses_notimeout'))
	{
		echo "FAIL\nncurses_notimeout does not exist\n";
		$fail = true;
	}
	if (!function_exists('ncurses_gettimeout'))
	{
		echo "FAIL\nncurses_gettimeout does not exist\n";
		$fail = true;
	}

	if (!function_exists('ncurses_gettermsize'))
	{
		echo "FAIL\nncurses_gettermsize does not exist\n";
		$fail = true;
	}

	if (!$fail)
	{

		ncurses_init();

		list($y, $x) = ncurses_gettermsize();

		ncurses_addstr("terminal size = (y:$y, x:$x)\n\n");

		ncurses_addstr("terminal size = (y:$y, x:$x)\n\n");

		ncurses_addstr("ncurses_timeout = ".ncurses_gettimeout()."\n\n");

		ncurses_addstr("Setting timeout to 100\n");
		ncurses_timeout(100);
		ncurses_addstr("ncurses_timeout = ".ncurses_gettimeout()."\n\n");

		ncurses_refresh();
		loop();

		ncurses_addstr("\nRemoving timeout\n");
		ncurses_notimeout();
		ncurses_addstr("ncurses_timeout = ".ncurses_gettimeout()."\n\n");
		ncurses_refresh();
		loop();

		sleep(1);

		ncurses_end();
	}


$fail = false;
sleep(3);
}

/**************************
 *         memcache       *
 **************************/
if ($memcache)
{
	echo "/**************************
 *         memcache       *
 **************************/
";
}









/* Misc functions used for testing */

function sig($signo)
{
     echo "signal handler called\n";
}

function loop()
{
	ncurses_addstr("Looping. Press any key to continue ...\n");
	ncurses_refresh();
	while(ncurses_getch() === -1)
	{
		ncurses_addstr(".");
		ncurses_refresh();
	}
	ncurses_addstr("\nncurses_getch returned -1\n");
	ncurses_refresh();
}
?>
