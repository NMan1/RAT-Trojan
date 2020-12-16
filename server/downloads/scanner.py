import nmap
import socket
import sys
from getmac import get_mac_address


class Network(object):
    def __init__(self, ip, network_name):
        self.ip = ip
        self.network_name = network_name

    def get_mac_address(self, host):
        mac = get_mac_address(ip=host)
        return mac

    def scan(self):
        network = self.ip + "/24"
        print("Scanning All Devices On Network: " + self.network_name[9:] +  "------->")

        nm = nmap.PortScanner()
        nm.scan(hosts=network, arguments="-sn")
        hosts_list = [(x, nm[x]['status']['state']) for x in nm.all_hosts()]
        for host, status in hosts_list:
            try:
                print(f"Host\t{socket.gethostbyaddr(host)[0]}\t{host}\t{self.get_mac_address(host)}\t{status}")
            except Exception:
                print(f"Host\tNone\t{host}\t{self.get_mac_address(host)}\t{status}")


if __name__ == '__main__':
    n = Network(sys.argv[1], sys.argv[2])
    n.scan()