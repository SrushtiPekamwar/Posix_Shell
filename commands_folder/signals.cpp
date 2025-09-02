// 10.Simple signals
// 1.
// CTRL-Z It should push any currently running foreground job into the background, and change its state
// from running to stopped. This should have no eﬀect on the shell if there is no foreground process running.
// 2.
// CTRL-C It should interrupt any currently running foreground job, by sending it the SIGINT signal. This
// should have no eﬀect on the shell if there is no foreground process running.
// 3.
// CTRL-D It should log you out of your shell, without having any eﬀect on the actual terminal.