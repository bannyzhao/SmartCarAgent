#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IPSTR "192.168.3.39" //������IP��ַ;
#define PORT 9000
#define BUFSIZE 1024

// base64 ת����, ��64��
static const char base64_alphabet[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'+', '/' };

int base64_encode(const char *indata, int inlen, char *outdata, int *outlen) {

	int ret = 0; // return value
	if (indata == NULL || inlen == 0) {
		return ret = -1;
	}

	int in_len = 0; // Դ�ַ�������, ���in_len����3�ı���, ��ô��Ҫ����3�ı���
	int pad_num = 0; // ��Ҫ������ַ�����, ����ֻ��2, 1, 0(0�Ļ�����Ҫƴ��, )
	if (inlen % 3 != 0) {
		pad_num = 3 - inlen % 3;
	}
	in_len = inlen + pad_num; // ƴ�Ӻ�ĳ���, ʵ�ʱ�����Ҫ�ĳ���(3�ı���)

	int out_len = in_len * 8 / 6; // �����ĳ���

	char *p = outdata; // ����ָ��ָ�򴫳�data���׵�ַ

					   //����, ����Ϊ������ĳ���, 3�ֽ�һ��
	for (int i = 0; i < in_len; i += 3) {
		int value = *indata >> 2; // ��indata��һ���ַ������ƶ�2bit(����2bit)
		char c = base64_alphabet[value]; // ��Ӧbase64ת������ַ�
		*p = c; // ����Ӧ�ַ�(������ַ�)��ֵ��outdata��һ�ֽ�

				//�������һ��(���3�ֽ�)������
		if (i == inlen + pad_num - 3 && pad_num != 0) {
			if (pad_num == 1) {
				*(p + 1) = base64_alphabet[(int)(cmove_bits(*indata, 6, 2) + cmove_bits(*(indata + 1), 0, 4))];
				*(p + 2) = base64_alphabet[(int)cmove_bits(*(indata + 1), 4, 2)];
				*(p + 3) = '=';
			}
			else if (pad_num == 2) { // ����������Ҫ������ '='
				*(p + 1) = base64_alphabet[(int)cmove_bits(*indata, 6, 2)];
				*(p + 2) = '=';
				*(p + 3) = '=';
			}
		}
		else { // ����������3�ֽڵ�����
			*(p + 1) = base64_alphabet[cmove_bits(*indata, 6, 2) + cmove_bits(*(indata + 1), 0, 4)];
			*(p + 2) = base64_alphabet[cmove_bits(*(indata + 1), 4, 2) + cmove_bits(*(indata + 2), 0, 6)];
			*(p + 3) = base64_alphabet[*(indata + 2) & 0x3f];
		}

		p += 4;
		indata += 3;
	}

	if (outlen != NULL) {
		*outlen = out_len;
	}

	return ret;
}

int getImage()
{
	return 0;
}


int main(int argc, char **argv)
{
	int sockfd, ret, i, h;
	struct sockaddr_in servaddr;
	char str1[4096], str2[4096], buf[BUFSIZE], *str;
	socklen_t len;
	fd_set   t_set1;
	struct timeval  tv;

	//�����׽���
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("������������ʧ��,���̼߳�����ֹ---socket error!\n");
		exit(0);
	};

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, IPSTR, &servaddr.sin_addr) <= 0) {
		printf("������������ʧ��,���̼߳�����ֹ--inet_pton error!\n");
		exit(0);
	};

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("���ӵ�������ʧ��,connect error!\n");
		exit(0);
	}
	printf("��Զ�˽���������\n");
	memset(str2, 0, 4096);
	strcat(str2, "theDataToPost");
	str = (char *)malloc(128);
	len = strlen(str2);
	sprintf(str, "%d", len);

	memset(str1, 0, 4096);
	strcat(str1, "GET /getAction HTTP/1.1\n");
	strcat(str1, "Host: 192.168.3.39\n");
	strcat(str1, "Content-Type: application/json\n");
	strcat(str1, "Content-Length: ");
	strcat(str1, str);
	strcat(str1, "\n\n");

	strcat(str1, str2);
	strcat(str1, "\r\n\r\n");
	printf("%s\n", str1);

	while (1) {
		printf("start again.\r\n");
		sleep(2);

		ret = write(sockfd, str1, strlen(str1));
		if (ret < 0) {
			printf("����ʧ�ܣ����������%d��������Ϣ��'%s'\n", errno, strerror(errno));
			exit(0);
		}
		else {
			printf("��Ϣ���ͳɹ�����������%d���ֽڣ�\n\n", ret);
		}

		FD_ZERO(&t_set1);
		FD_SET(sockfd, &t_set1);



		tv.tv_sec = 0;
		tv.tv_usec = 0;
		h = 0;
		printf("--------------->1\r\n");
		h = select(sockfd + 1, &t_set1, NULL, NULL, &tv);
		printf("--------------->2\r\n");

		//if (h == 0) continue;
		if (h < 0) {
			printf("--------------->3\r\n");
			close(sockfd);
			printf("�ڶ�ȡ���ݱ���ʱSELECT��⵽�쳣�����쳣�����߳���ֹ��\n");
			return -1;
		};

		if (h > 0) {
			printf("--------------->4\r\n");
			memset(buf, 0, 4096);
			printf("--------------->5\r\n");
			i = read(sockfd, buf, 4095);
			printf("--------------->6\r\n");
			if (i == 0) {
				close(sockfd);
				printf("��ȡ���ݱ���ʱ����Զ�˹رգ����߳���ֹ��\n");
				return -1;
			}

			printf("read result:%s\n", buf);
		}
	}
	close(sockfd);


	return 0;
}
