小谈RTMP中AMF3类型的Command message
由于历史原因我喜欢将这个消息叫做invoke。

 

1、AMF3类型的Invoke消息的类型是17，而AMF0的Invoke消息的消息类型是20。

 2、AMF3类型的Invoke也是使用AMF0进行序列化，只是其中的object使用AFM0类型AMF0_AMF3_OBJECT(0x11)，也就是body部分是一个 采用AFM3格式的object。

3、AMF3类型Invoke消息体的第一个字节是个无用的0字节，第二个字节才是command name(以0x2开头)