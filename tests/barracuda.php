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

	if (!function_exists('ncurses_newterm'))
	{
		echo "FAIL\nncurses_newterm does not exist\n";
		$fail = true;
	}

	if (!function_exists('ncurses_set_term'))
	{
		echo "FAIL\nncurses_set_term does not exist\n";
		$fail = true;
	}

	if (!function_exists('ncurses_delscreen'))
	{
		echo "FAIL\nncurses_delscreen does not exist\n";
		$fail = true;
	}

	if (!$fail)
	{

		/* test with ncurses_init and ncurses_end */
		echo "Opening terminal with ncurses_init\n";
		sleep(1);
		ncurses_init();

		ncurses_start_color();
		ncurses_init_pair(1, NCURSES_COLOR_RED, NCURSES_COLOR_BLACK);
		ncurses_color_set(1);
		ncurses_border(0,0,0,0,0,0,0,0);
		ncurses_color_set(0);
		ncurses_refresh();
		write_to_screen();
		ncurses_end();

		/* test with ncurses_newterm and ncurses_delscreen */
		echo "Opening terminal with ncurses_newterm\n";
		sleep(1);
		$term1 = ncurses_newterm("/proc/self/fd/1", "/proc/self/fd/0"); // might not always be /proc/self/fd/1|0
		ncurses_set_term($term1);

		ncurses_start_color();
		ncurses_init_pair(1, NCURSES_COLOR_GREEN, NCURSES_COLOR_BLACK);
		ncurses_color_set(1);
		ncurses_border(0,0,0,0,0,0,0,0);
		ncurses_color_set(0);
		ncurses_refresh();
		ncurses_refresh();
		write_to_screen();
		ncurses_end();

		echo "Opening another terminal with ncurses_newterm\n";
		sleep(1);
		$term2 = ncurses_newterm("/proc/self/fd/1", "/proc/self/fd/0"); // might not always be /proc/self/fd/1|0
		ncurses_set_term($term2);
		ncurses_start_color();
		ncurses_init_pair(1, NCURSES_COLOR_BLUE, NCURSES_COLOR_BLACK);
		ncurses_color_set(1);
		ncurses_border(0,0,0,0,0,0,0,0);
		ncurses_color_set(0);
		ncurses_refresh();
		ncurses_refresh();
		write_to_screen();
		ncurses_end();

		/* Delete screens in the reverse order they were opened in when they reference the same terminal */
		delete_screen($term2);
		delete_screen($term1);
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

function loop($i)
{
	ncurses_mvaddstr(($i+2), 1, "Looping. Press any key to continue ...");
	ncurses_refresh();
	while(ncurses_getch() === -1)
	{
		ncurses_addstr(".");
		ncurses_refresh();
	}
	ncurses_mvaddstr(($i+4), 1, "ncurses_getch returned -1");
	ncurses_refresh();
}

function write_to_screen()
{
		list($y, $x) = ncurses_gettermsize();

		ncurses_mvaddstr(1, 1, "terminal size = (y:$y, x:$x)");

		ncurses_mvaddstr(3, 1, "terminal size = (y:$y, x:$x)");

		ncurses_mvaddstr(5, 1, "ncurses_timeout = ".ncurses_gettimeout());

		ncurses_mvaddstr(7, 1, "Setting timeout to 100");
		ncurses_timeout(100);
		ncurses_mvaddstr(8, 1, "ncurses_timeout = ".ncurses_gettimeout());

		ncurses_refresh();
		loop(8);

		ncurses_mvaddstr(13, 1, "Removing timeout");
		ncurses_notimeout();
		ncurses_mvaddstr(14, 1, "ncurses_timeout = ".ncurses_gettimeout());
		ncurses_refresh();
		loop(14);

		sleep(1);
}

function delete_screen(&$screen)
{
	ncurses_set_term($screen);
	ncurses_end();
	ncurses_delscreen($screen);
}
?>
