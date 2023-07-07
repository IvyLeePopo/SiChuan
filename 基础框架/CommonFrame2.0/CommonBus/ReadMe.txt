2017-11-14 lijin

1:


2017-11-8  lijin

1:新增发布参数：srcKey（发布源key）
由原来的
 virtual void send2Bus(int destKey,char *msg,int len);
改成
 virtual void send2Bus(int destKey,char *msg,int len,int srcKey=0);

用于满足 区分同一组件处理不同源的处理逻辑


2：新增订阅参数：srcKey（发布源key）
由原来的
virtual void recvFromBus(int destKey,char *msg,int len);
改成
virtual void recvFromBus(int destKey,char *msg,int len, int srcKey=0);

用于满足 区分同一组件处理不同源的处理逻辑

3：新增 KEY定义，后续需要增加的请在 enum COMMPENTS_KEY_TYPE 里边新增 。
