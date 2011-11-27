#!/bin/sh

#set -x

#default parameters, when only value is provided
myPachubeKey="oKNoikCFuVjLsVDq4t7aCT28InZKiN818zhNionj8y0"
feedid="40546";

host="api.pachube.com";
port="80";
path="/v2/feeds/${feedid}";

ok="true"
if [ $# -ge 1 ]; then
	currentVal=$1;
	if [ $# -ge 2 ]; then
		host="$2";
		if [ $# -ge 3 ]; then
			port="$3";
			if [ $# -eq 4 ]; then
				path="$4";
			else
				ok="false"
			fi;
		fi;
	fi;
else	
	ok="false"
fi

if [ "$ok" != "true" ]; then
	echo "Usage:
	<current-value> [<host-name> [<port-num> [<path>]]] (from 1 to 3 parameters, got ${#})";
	exit 1;
fi;

data="{
	\"version\":\"1.0.0\",
  	\"datastreams\":[
      		{\"id\":\"1_id\", \"current_value\":\"${1}\"}
  ]
}

"

headers="PUT ${path} HTTP/1.1
User-Agent: curl/7.21.6 (x86_64-pc-linux-gnu) libcurl/7.21.6 OpenSSL/1.0.0e zlib/1.2.3.4 libidn/1.22 librtmp/2.3
Host: ${host}
Accept: */*
X-PachubeApiKey: ${myPachubeKey}
Content-Length: ${#data}
Content-Type: application/x-www-form-urlencoded

"


#curl --request PUT --data-binary "${data}" --header "X-PachubeApiKey: oKNoikCFuVjLsVDq4t7aCT28InZKiN818zhNionj8y0" --verbose 192.168.2.107:9999
#curl --request PUT --data-binary "${data}" --header "X-PachubeApiKey: ${myPachubeKey}" --verbose ${host}${path}

dataToSend=${headers}${data};
#echo -e "\n\n\n\\n\n"
#echo "$dataToSend"
echo "$dataToSend" | telnet ${host} ${port}
#echo "$dataToSend" | telnet 192.168.2.107 9999
