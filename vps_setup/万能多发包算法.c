/* 编译方法:gcc -O2 -o fuck_net fuck_net.c -lpcap -lnet */
/* 依赖:apt install libnet1-dev libpcap0.8-dev */

#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <libnet.h>

/* 捕捉长度 */
#define SNAP_LEN 65535
#define ETHERNET_H_LEN 16
#define SPECIAL_TTL 88

/* 多发倍数 */
#define REPACK_TIME 5

//成功捕获包的函数.这里已经自动拆开成数据包,以太网头等信息.
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
	static int count = 1;                  
	struct libnet_ipv4_hdr *ip; 
	int i = 0;
	libnet_t *libnet_handler[REPACK_TIME];	

	// 等下可以用来发包的句柄.
	for(int i = 0;i < REPACK_TIME;i++){
		libnet_handler[i] = (libnet_t *)args;
	}
	count++;
	
	// 得到IP包头地址
	ip = (struct libnet_ipv4_hdr*)(packet + ETHERNET_H_LEN);

	if(ip->ip_ttl != SPECIAL_TTL) {
		// 如果数据包已经被捕捉,那么改变他TTL,否则,会无限次循环发送.
		ip->ip_ttl = SPECIAL_TTL;
		// 暴力多发N次数据包.不管成功与失败.这就是多发N倍,损人利己.因为数据包有重发机制,这里重发很多份,后果就是哪份到达了你就要了哪份.
		for(i = 0;i < REPACK_TIME;i++){
			libnet_adv_write_raw_ipv4(libnet_handler[i], (u_int8_t *)ip, ntohs(ip->ip_len));
		}
		
	}
	
	return;
}

int main(int argc, char **argv) {
	char errbuf_pcap[PCAP_ERRBUF_SIZE];
	char errbuf_libnet[LIBNET_ERRBUF_SIZE];
	pcap_t *handle;

	struct bpf_program fp;
	bpf_u_int32 net, mask;
	
    // 调用libpcap的库函数来检查设备的子网掩码和IP地址.
	if (pcap_lookupnet("venet0", &net, &mask, errbuf_pcap) == -1) {
		net = 0;
		mask = 0;
	}

    // 打开一个pcap会话句柄,每1秒捕获一次数据包.
	handle = pcap_open_live("venet0", SNAP_LEN, 1, 1000, errbuf_pcap);
	if(handle == NULL) {
		return -1;
	}

	// 用于发数据包的工具.
	libnet_t *libnet_handler = libnet_init(LIBNET_RAW4_ADV, "venet0", errbuf_libnet);
	if(NULL == libnet_handler) {
		return -1;
	}

	// 下面继续调用libpcap的函数,把过滤规则转换为BPF底层规则.
	if (pcap_compile(handle, &fp, "ip", 0, net) == -1) {
		return -1;
	}

	// 过滤出需要多发的包.
	if (pcap_setfilter(handle, &fp) == -1) {
		return -1;
	}
	
	// 循环捕获数据包
	while(1) {
		pcap_loop(handle, 1, got_packet, (u_char *)libnet_handler);
	}

	/* 退出清理 */
	pcap_freecode(&fp);
	pcap_close(handle);
	libnet_destroy(libnet_handler);
	return 0;
}