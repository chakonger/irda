# 红外服务IKA0接口demo

## 获取固定码和变化码的命令 IKA0

IKAO的接口主要适用于配合我们的软件接口使用，调用此接口可以直接输出固定码和相应的命令参数。 

###案例
请求：
```
   {"CMD":"IKA0","TOKEN":"4FF32895E665D89722C5","typeID":"100001", "inst":"100800"}
```

|字段|字段说明|备注|
|:---|:---|:---|
|CMD|命令类型|
|typeID|红外类型代码|


返回:

```
{"CMD":"IKB0","fixParam":"260104b80bb80bb80b941102260226020226027c06022602204e00",
"param":[{"delay":"0","actionID":"6","param":"017000653100000205000000010000A0E90000"}]}
```

|字段|字段说明|备注|
|:---|:---|:---|
|CMD|命令类型|返回IKB0，表示成功|
|typeID|红外类型代码|
