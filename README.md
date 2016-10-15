# epoll


##

epoll的接口非常简单，一共就三个函数： 
1. int epoll_create(int size); 
 创建一个epoll的句柄，size用来告诉内核这个监听的数目一共有多大。这个参数不同于select()中的第一个参数，给出最大监听的fd+1的值。
 需要注意的是，当创建好epoll句柄后，它就是会占用一个fd值，在linux下如果查看/proc/进程id/fd/，是能够看到这个fd的，所以在使用完epoll后，
 必须调用close()关闭，否则可能导致fd被耗尽.
 
 
2. int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event); 
epoll的事件注册函数，它不同与select()是在监听事件时告诉内核要监听什么类型的事件，而是在这里先注册要监听的事件类型。第一个参数是epoll_create()的返回值，第二个参数表示动作，用三个宏来表示： 
EPOLL_CTL_ADD：注册新的fd到epfd中； 
EPOLL_CTL_MOD：修改已经注册的fd的监听事件； 
EPOLL_CTL_DEL：从epfd中删除一个fd； 
第三个参数是需要监听的fd，第四个参数是告诉内核需要监听什么事，struct epoll_event结构如下： 
struct epoll_event { 
__uint32_t events; /* Epoll events */ 
epoll_data_t data; /* User data variable */ 
}; 

events可以是以下几个宏的集合： 
EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）； 
EPOLLOUT：表示对应的文件描述符可以写； 
EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）； 
EPOLLERR：表示对应的文件描述符发生错误； 
EPOLLHUP：表示对应的文件描述符被挂断； 
EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。 
EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里 

3. int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout); 
等待事件的产生，类似于select()调用。参数events用来从内核得到事件的集合，maxevents告之内核这个events有多大，
这个maxevents的值不能大于创建epoll_create()时的size，
参数timeout是超时时间（毫秒，0会立即返回，-1，永久阻塞，直到某个事件发生）。该函数返回需要处理的事件数目，如返回0表示已超时。

LT(level triggered)是缺省的工作方式，并且同时支持block和no-block socket.
在这种做法中，内核告诉你一个文件描述符是否就绪了，然后你可以对这个就绪的fd进行IO操作。如果你不作任何操作，内核还是会继续通知你的，
所以，这种模式编程出错误可能性要小一点。传统的select/poll都是这种模型的代表． 

ET(edge-triggered)是高速工作方式，只支持no-block socket。
在这种模式下，当描述符从未就绪变为就绪时，内核通过epoll告诉你。
然后它会假设你知道文件描述符已经就绪，并且不会再为那个文件描述符发送更多的就绪通知，
直到你做了某些操作导致那个文件描述符不再为就绪状态了(比如，你在发送，接收或者接收请求，或者发送接收的数据少于一定量时导致了一个EWOULDBLOCK 错误）。
但是请注意，如果一直不对这个fd作IO操作(从而导致它再次变成未就绪)，内核不会发送更多的通知(only once),
不过在TCP协议中，ET模式的加速效用仍需要更多的benchmark确认（这句话不理解）。

总的来说：
     LT模式：当epoll_wait检测到描述符事件发生并将此事件通知应用程序，应用程序可以不立即处理该事件。
     下次调用epoll_wait时，会再次响应应用程序并通知此事件。直到你做了为止。
　　ET模式：当epoll_wait检测到描述符事件发生并将此事件通知应用程序，应用程序必须立即处理该事件。只通知你一次，不管你做不做。
  如果不处理，下次调用epoll_wait时，不会再次响应应用程序并通知此事件。
  
  
  
  
  
