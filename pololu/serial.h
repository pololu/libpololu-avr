void serial_set_baud_rate(unsigned long baud);
void serial_set_mode(unsigned char mode);
void serial_check();
void serial_receive(char *buffer, unsigned char size);
void serial_cancel_receive();
char serial_receive_blocking(char *buffer, unsigned char size, unsigned int timeout);
void serial_receive_ring(char *buffer, unsigned char size);
unsigned char serial_get_received_bytes();
char serial_receive_buffer_full();
void serial_send(char *buffer, unsigned char size);
void serial_send_blocking(char *buffer, unsigned char size);
unsigned char serial_get_sent_bytes();
char serial_send_buffer_empty();

#define SERIAL_AUTOMATIC 0
#define SERIAL_CHECK 1
