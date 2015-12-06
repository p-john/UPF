from scapy.all import *
import threading
import time

def main():
	src_mac = "aa:bb:cc:dd:ee:ff"
	dst_mac = "b8:27:eb:03:80:c0"
	src_ip = "12.168.9.1"
	dst_ip = "122.168.99.1"
	tcp_packet = Ether(src=src_mac, dst=dst_mac)/\
			IP(src=src_ip, dst=dst_ip)/TCP(sport=1, dport=2)

	udp_packet = Ether(src=src_mac, dst=dst_mac)/\
			IP(src=src_ip, dst=dst_ip)/UDP(sport=1, dport=2)

	icmp_packet = Ether(src=src_mac, dst=dst_mac)/\
			IP(src=src_ip, dst=dst_ip)/ICMP()
	packets = [tcp_packet, udp_packet, icmp_packet]

	threading.Thread(target=send_packets,args=(packets,"eth4")).start()
	threading.Thread(target=send_packets,args=(packets,"eth3")).start()
	threading.Thread(target=send_packets,args=(packets,"eth2")).start()


if __name__ == "__main__":
	main()


def send_packets(packets,interface):
	start = time.time()
	while time.time() < start + 10:
		sendp(packets,iface=interface, loop=1)