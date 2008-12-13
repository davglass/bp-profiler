This directory contains BrowserPlus ruby service that is a proof of 
concept of a service that would audit both memory usage and processor
usage of a browser to make deeper information available about resource
consumption of a browser while running a particular site.

Right now it's a hack, no, a kludge that's brittle, ugly and smells bad.
It's osx/ffx only.

How to get the service installed:

1. install BrowserPlus: http://browserplus.yahoo.com/install
2. install ruby interpreter service (by say, running the Json demo or
   using the service explorer)
3. copy the BrowserProfiler service into
  ~/Library/Application\ Support/Yahoo\!/BrowserPlus/Corelets/BrowserProfiler/0.0.1/
4. restart browserplus (can do it from system preferences -> BrowserPlus)
5. open test service_test.htm
