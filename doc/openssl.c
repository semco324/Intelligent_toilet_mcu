头文件
#include<openssl/ssl.h>
#include<openssl/err.h>
openssl使用步骤
1.openssl 初始化
	int SSL_library_init(void);

2.选择会话协议
	SSL_METHOD* method = TLSv1_1_client_method();
	
3.创建会话环境
   在openssl中创建的ssl会话环境称为CTX，使用不同的会话协议，环境不一样
	申请ssl会话环境:
	SSL_CTX *SSL_CTX_new(SSL_METHOD* method);
	当ssl会话环境申请成功后，还要根据实际需要设置CTX属性，通常的设置是指定ssl
	握手阶段证书的验证方式和加载自己的证书
	
	制定证书验证方式的函数：
	void SSL_CTX_set_verify(SSL_CTX *ctx, int mode, int (*verify_callback)(int, X509_STORE_CTX *));
	
	为ssl会话环境加载CA证书函数：
	SSL_CTX_load_vertify_location(SSL_CTX *ctx,const char*Cafile,const char*Capath);
	
	为ssl会话环境加载用户证书函数：
	SSL_CTX_use_certificate_file(SSL_CTX *ctx,const char*file,int type);
	
	为ssl会话环境加载用户私钥函数：
	SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx,const char*file,int type);
	
	在将证书和私钥加载到ssl会话环境之后，就可以调用下面函数来验证私钥和证书是否相符合
	int SSL_CTX_check_private_key(SSL_CTX * ctx);
	
4.建立ssl套接字
	ssl套接字是建立在普通tcp套接字基础之上，在建立ssl套接字时可以使用下面函数
	
	SSL* SSL_new(SSL_CTX *ctx);  //申请一个ssl套接字
	
	int SSL_set_fd(SSL* ssl,int fd); //绑定读写套接字
	
	int SSL_set_rfd(SSL* ssl,int fd); //绑定只读套接字
	
	int SSL_set_wfd(SSL* ssl,int fd); //绑定只写套接字
	
5.完成SSL握手
	在成功创建ssl套接字后，客户端应使用SSL_connect()替代connect()完成握手
	
	int SSL_connect(SSL* ssl);
	
	对服务器来讲,则应该使用SSL_accept()替代accept()完成握手
	
	int SSL_accept(SSL* ssl);
	
	握手完成后，通常需要询问通讯双发的证书信息，以便进行相应验证，使用以下函数
	
	X509 *SSL_get_peer_certificate(SSL*ssl);
	
	该函数可以从SSL套接字中提取对方证书信息，这些信息被SSL验证过
	
	X509_NAME*X509_get_subject_name(X509* a);
	该函数得到证书所用者名字
	
6.进行数据传输
	当SSL握手完成后，就可以进行数据传输，使用SSL_read()和SSL_write()代替传统的read()和write()
	int SSL_read(SSL* ssl,void* buf,int num);
	int SSL_write(SSL* ssl,void* buf,int num);
	
7.结束SSL通讯
	当客户端和服务器之间的数据通讯完成之后，调用下面函数来释放已经申请的SSL资源
	
	int SSL_shutdown(SSL* ssl); //关闭SSL套接字
	
	void SSL_free(SSL* ssl); //释放SSL套接字
	
	void SSL_CTX_free(SSL_CTX *ctx); //释放SSL会话环境
	
	
	
客户端框架
#define OPENSSL_DEMO_TARGET_NAME "www.baidu.com"
#define OPENSSL_DEMO_FRAGMENT_SIZE 8192
#define OPENSSL_DEMO_LOCAL_TCP_PORT 1000
#define OPENSSL_DEMO_TARGET_TCP_PORT 443
#define OPENSSL_DEMO_RECV_BUF_LEN 1024
#define OPENSSL_DEMO_REQUEST "{\"path\": \"/v1/ping/\", \"method\": \"GET\"}\r\n"
                            //"{
							//	"path": "/v1/ping/", 
							//	"method": "GET"
							// }\r\n"
void openssl_client_demo(void *arg)
{
    //生成一个SSL结构
    SSL_CTX *ctx;
    SSL *ssl;

    struct sockaddr_in sock_addr;
    ip_addr_t target_ip;

	do{
		int ret = netconn_gethostbyname(OPENSSL_DEMO_TARGET_NAME, &target_ip);
	}while(ret);
    os_printf("get target IP is %d.%d.%d.%d\n", (unsigned char)((target_ip.addr & 0x000000ff) >> 0),
                                                (unsigned char)((target_ip.addr & 0x0000ff00) >> 8),
                                                (unsigned char)((target_ip.addr & 0x00ff0000) >> 16),
                                                (unsigned char)((target_ip.addr & 0xff000000) >> 24));
	
	os_printf("create SSL context ......\r\n");
	if( !(ctx = SSL_CTX_new(TLSv1_1_client_method())) )
	{
        os_printf("failed\n");
        goto failed1;	
	}
    os_printf("create SSL context OK\n");	
	
    os_printf("set SSL context read buffer size ......\r\n");
    SSL_CTX_set_default_read_buffer_len(ctx, OPENSSL_DEMO_FRAGMENT_SIZE);

	//正常socket过程
	os_printf("create socket ......");
	if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	   os_printf("create socket err......\r\n");
	   goto failed2;
	}
	os_printf("bind socket ......");
	memset(&sock_addr,0,sizeof(struct sockaddr_in));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(OPENSSL_DEMO_LOCAL_TCP_PORT);
	if( bind(socketFd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
	{
	  os_printf("bind socket err......\r\n");
	  goto failed3;
	}
	
    os_printf("socket connect to remote ......");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = target_ip.addr;
    sock_addr.sin_port = htons(OPENSSL_DEMO_TARGET_TCP_PORT);	
	if( connect(socketFd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
	{
	  os_printf("bind socket err......\r\n");
	  goto failed4;
	}
	
	os_printf("create SSL ......");
	if(!(ssl = SSL_new(ctx)))
	{
       os_printf("ssl err\n");
       goto failed5;	
	}
	
    //把创建好的ssl和socket联系起来
    SSL_set_fd(ssl, socketFd);	
	
    os_printf("SSL connected to %s port %d ......", OPENSSL_DEMO_TARGET_NAME, OPENSSL_DEMO_TARGET_TCP_PORT);

	//ssl握手过程
	if(!(ret=SSL_connect(ssl)))
	{
        os_printf("failed, return [-0x%x]\n", -ret);
        goto failed6;	
	}
    os_printf("send request to %s port %d ......", OPENSSL_DEMO_TARGET_NAME, OPENSSL_DEMO_TARGET_TCP_PORT);
	
	//用SSL_write(),SSL_read() 通讯
    os_printf("send request to %s port %d ......", OPENSSL_DEMO_TARGET_NAME, OPENSSL_DEMO_TARGET_TCP_PORT);
    char send_data[] = OPENSSL_DEMO_REQUEST;
	ret = SSL_write(ssl, send_data, sizeof(send_data));
	if(ret <= 0 )
	{
        os_printf("SSL_write failed, return [-0x%x]\n", -ret);
        goto failed7;	
	}
	
	int recv_bytes = 0;
    char recv_buf[OPENSSL_DEMO_RECV_BUF_LEN];
	while(1)
	{
		ret = SSL_read(ssl, recv_buf, OPENSSL_DEMO_RECV_BUF_LEN - 1);
		if(ret <= 0) break;
		recv_bytes += ret;
		os_printf("%s", recv_buf);
	}
    os_printf("read %d bytes data from %s ......\n", recv_bytes, OPENSSL_DEMO_TARGET_NAME);	
failed7:
    SSL_shutdown(ssl);
failed6:
    SSL_free(ssl);
failed5:
failed4:
failed3:
    close(socket);
failed2:
    SSL_CTX_free(ctx);
failed1:
    vTaskDelete(NULL);
    os_printf("task exit\n");
    return ;	
}
	

	

    

	

	


服务器框架
    //生成一个SSL结构
    SSL_CTX *ctx;
	ctx = SSL_CTX_new(TLSv1_1_server_method());
    SSL_CTX_set_default_read_buffer_len(ctx, OPENSSL_DEMO_FRAGMENT_SIZE);
	ssl = SSL_new(ctx);
	
	//正常socket过程
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
	bind()
	listen()
	accept()
	
    //把创建好的ssl和socket联系起来
    SSL_set_fd(ssl, socketFd);

	//ssl握手过程
	SSL_connect(ssl);

	//用SSL_write(),SSL_read() 通讯
	SSL_read(ssl,buf,sizeof(buf));
	
	
http://www.docin.com/p-532641332.html

https://wenku.baidu.com/view/74575278ed630b1c58eeb517.html?sxts=1531385965326
	
	
	
	
	
	
	