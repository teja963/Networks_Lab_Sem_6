gcc mailclient.c -o clienttcp
gcc stmpmail.c -o servertcp

./servertcp
./clienttcp port_no

