<?php

/**************************
 *           pcntl        *
 **************************/
echo "
/**************************
 *           pcntl        *
 **************************/
";

echo "Installing signal handler...\n";
pcntl_signal(SIGHUP, 'sig');

function sig($signo)
{
     echo "signal handler called\n";
}

echo "Generating signal SIGHUP to self...\n";
posix_kill(posix_getpid(), SIGHUP);

if (function_exists('pcntl_signal_dispatch'))
{
	echo "pcntl_signal_dispatch exists, calling it...\n";
	pcntl_signal_dispatch();
}
else
{
	echo "pcntl_signal_dispatch does not exist, generating tick...\n";
	declare(ticks = 1);
	usleep(1);
}

sleep(3);

/**************************
 *         ncurses        *
 **************************/
echo "
/**************************
 *         ncurses        *
 **************************/
";

ncurses_init();

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

sleep(3);

/**************************
 *         memcache       *
 **************************/
echo "/**************************
 *         memcache       *
 **************************/
";
?>
