#	This is the default standard profile provided to a user.
#	They are expected to edit it to meet their own needs.

MAIL=/usr/mail/${LOGNAME:?}

PATH=$PATH:/usr/sbin:/usr/lib/uucp:/usr/ucb:.
PS1=`whoami`':$PWD !> '
TZ=PST8PDT
export PATH TZ
echo " "
set -o vi
alias a=alias
a lsa="ls -las"
a hi=history
a cpout="cpio -ouvB -O /dev/rdsk/f0t"
a cpin="cpio -iuvdB -I /dev/rdsk/f0t"
a dira="cpio -ivt < /dev/rdsk/f0t"
a cls=clear
a job="jobs -l"
a ff="fortune -a"
a fo="fortune -o"
