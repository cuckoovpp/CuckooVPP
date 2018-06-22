# CuckooVPP

Longest Prefix Matching algorithm in VPP for fast and scalable routing lookup.

## Getting Started

These instructions will allow you to copy the project and you are able to run on your local machine for development and testing purposes.

### Prerequisites

Followings are prerequisites you need to have:
```
Operatin System: 14.04.5LTS Ubuntu. You can get from http://releases.ubuntu.com/14.04/
VPP:vpp17.07 rc0 use git clone https://gerrit.fd.io/r/vpp -b v17.07-rc0
Cuckoo files: cuckoo.c and cuckoo.h
```

### Installing

A step by step instructions that tell you how to install and run CuckooVPP

```
1) Run the command pstree –p to check if init is ps 1.
If not you are probably using systemd. Install upstart instead
2) Go to your install directory for vpp. Run the following command:
git clone https://gerrit.fd.io/r/vpp
3) Enter directory vpp. Run command “git describe” to check the version of vpp.
4) Navigate to vpp/build-root/vagrant
5) Run the command ./build.sh. Wait for the build process. Look out for errors. Vpp-api-python error can be ignored. Though if its required install python dev package.
6) Execute sudo dpkg –i /vpp/build-root/*.deb 
6) Read startup.conf file. Run the command: more /etc/vpp/startup.conf. Read the comments and understand the sections of the file, especially NIC whitelists

```

To integrate cuckoo use following steps:

```
Putting​ ​ the​ ​ cuckoo​ ​ files​ ​ into​ ​ vpp.
The​ ​ /vpp/src/vnet/util​​ ​ folder​ ​ contains​ ​ the​ ​ cuckoo​ ​ files
Also​ ​ it​ ​ is​ ​ not​ ​ enough​ ​ to​ ​ simply​ ​ copy​ ​ the​ ​ files​ ​ into​ ​ the​ ​ above​ ​ location.
We​ ​ need​ ​ to​ ​ modify​ ​ the​ ​ “ ​ vnet.am​ ” ​ ​ file​ ​ in​ ​ /vpp/src/​ ​ ​ as​ ​ below
nobase_include_HEADERS​ ​ +=​ ​ cuckoo.h
libvnet_la_SOURCES​ ​ +=​ ​ cuckoo.c
```

```
Now​ ​ we​ ​ have​ ​ call​ ​ our​ ​ cuckoo​ ​ filter​ ​ functions​ ​ to​ ​ create​ ​ the​ ​ filter​ ​ and​ ​ insert​ ​ routes​ ​ into​ ​ the
filter​ ​ and​ ​ do​ ​ lookups​ ​ in​ ​ the​ ​ filter​ ​ etc.
To​ ​ do​ ​ these ​ we​ ​ need​ ​ to​ ​ modify​ ​ the​ ​ ip6_forward.c​​ ​ file​ ​ in​​ ​ /src/vnet/ip​​ ​ folder
Also​ ​ you​ ​ will​ ​ need​ ​ to​ ​ modify​ ​ the​ ​ ip.h,​ ​ ip6.h​ ​ , ​ ​ ip6_fib.c​ ​ , ​ ​ ip6_fib.h​ ​ , ​ ​ fib_table.h​ ​ files​ ​ under
the​ ​ /src/vnet/ip​​ ​ or​ ​ /src/vnet/fib​​ ​ folders.
```
## Running the tests

After installation of CuckooVPP is done, follow these steps to perform routing lookup:

```
sudo start vpp
sudo vppctl exec "file name to add routes in filter and routing table"
sudo vppctl clear lookup
sudo vppctl set cuckoo
sudo vppctl exec "lookup file"
sudo vppctl show ip6 lookup 
```

### Output

Above commands provide lookup time and number of cuckoo filter and FIB table lookup. Following is the result graph for different approaches:

![Output plot](https://github.com/krissnnaa/CuckooVPP/blob/master/lookup_rate.png)

## Deployment

Additional notes to deploy in real traffic environment
## Built With

* [Trex](http://trex-tgn.cisco.com) - The traffic generator


## Versioning

We use [Github](http://github.com/) for versioning. For the versions available, see the [this repository](https://github.com/cuckoovpp/CuckooVPP). 

## Authors

Anonymous


## Acknowledgments


