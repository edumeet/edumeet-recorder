# eduMeet Recorder

## Getting started

For example, given that you have the following URI:
 * https://letsmeet.no/test-edumeet-recorder

Steps to run the minimal example are:

  1. `git clone https://github.com/edumeet/edumeet-recorder.git`
  2. `cd edumeet-recorder`
  3. `docker build -t edumeet-recorder .` (few minutes to half an hour building)
  4. `docker run -v /tmp:/records --rm -it --network=host edumeet-recorder`
  5. Open in browser: https://letsmeet.no/test-edumeet-recorder
  6. Use curl to start recording:
```
curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"bitrate":6000, "uri":"https://letsmeet.no/test-edumeet-recorder?headless=true&displayName=recorder", "stream_uri":"","mode":"record"}}' -X POST http://127.0.0.1:34568/api/start
```
  7. Use curl stop to stop recording:
```
curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X POST http://127.0.0.1:34568/api/stop
```

Notes:
 * `headless=true` -- is used to skip the login/user/landing screen for eduMeet (a temporary hack)

## More examples and explanation

### Installation

- build the image:  
`docker build -t edumeet-recorder .`

- run the image:  
`docker run -v /tmp:/records --rm -it --network=host edumeet-recorder`

### Example usage

- Session 1:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"bitrate":6000,"uri":"https://letsmeet.no/roomname?headless=true&displayName=recorder","stream_uri":"","mode":"record"}}' -X POST http://127.0.0.1:34568/api/start`

HTTP/1.1 200 OK
Content-Length: 20
Content-Type: application/json

{"id":1,"status":"started"}

- Session 2:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"bitrate":6000,"uri":"https://letsmeet.no/roomname?headless=true&displayName=recorder","stream_uri":"","mode":"record"}}' -X POST http://127.0.0.1:34568/api/start`  

HTTP/1.1 200 OK
Content-Length: 20
Content-Type: application/json

{"id":2,"status":"started"}

- Status:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34568/api/status`  

HTTP/1.1 200 OK
Content-Length: 75
Content-Type: application/json

[{"id":1,"records":[],"running":true},{"id":2,"records":[],"running":true}]

- Stop:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X POST http://127.0.0.1:34568/api/stop`  

HTTP/1.1 200 OK
Content-Length: 20
Content-Type: application/json

{"status":"stopped"}

- Status:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34568/api/status`  

HTTP/1.1 200 OK
Content-Length: 109
Content-Type: application/json

[{"id":1,"records":["/records/2021-09-22_07:56:00__0"],"running":false},{"id":2,"records":[],"running":true}]

Stop 2:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":2}' -X POST http://127.0.0.1:34568/api/stop`  

HTTP/1.1 200 OK
Content-Length: 20
Content-Type: application/json

{"status":"stopped"}

- Status 2:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34568/api/status`  

HTTP/1.1 200 OK
Content-Length: 143
Content-Type: application/json

[{"id":1,"records":["/records/2021-09-22_07:56:00__0"],"running":false},{"id":2,"records":["/records/2021-09-22_07:57:58__1"],"running":false}]

### Recorder-service API

- get status:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34568/api/status`  
return:  
`[{"id":1,"is_running":true, "records":["/tmp/recorder_service/records/2020-09-04_13:19:15__0.webm"]}, {"id":2,"is_running":true}, {"id":3,"is_running":false}]`


- start raw rtp recordings:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"rtp", "data":{"rtpPort":5000,"rtcpPort":5001}}' -X POST http://127.0.0.1:34568/api/start`  
return:  
`{"status":"started"}`

- start html recordings / rtmp streaming:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"bitrate":6000,"uri":"https://videojs.github.io/autoplay-tests/plain/attr/autoplay.html","stream_uri":"rtmp://b.rtmp.youtube.com/live2/xxxx-xxxx-xxxx-xxxx-xxxx","mode":"record"}}' -X POST http://127.0.0.1:34568/api/start`  
return:  
`{"status":"started"}`  

    
  possible mode value:  
  `"mode":"record"`  
  `"mode":"stream"`  
  `"mode":"both"`  
  

- stop recordings / rtmp streaming:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X POST http://127.0.0.1:34568/api/stop`  
return:  
`{"status":"stopped"}`


- get recordings:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X GET http://127.0.0.1:34568/api/get_records`  
return:  
`["/tmp/recorder_service/records/2020-09-04_13:19:15__0.webm", "/tmp/recorder_service/records/2020-09-04_14:29:15__1.webm"]`

### Performance tests

Scenario: Connection to Edumeet with 3 participants.				
Encoder params: 1080p25 I420 8bit, ultrafast, bitrate 6Mbit, baseline

```
| cpu       | i7-4790K CPU @ 4.00GHz  | i7-7700K CPU @ 4.20GHz  | i7-11700K @ 3.60GHz  |
|-----------|-------------------------|-------------------------|----------------------|
| cores     | 8                       | 8                       | 16                   |
| ram       | 8                       | 16                      | 32                   |
|-----------|--------------------------------------------------------------------------|
|           | docker stats                                                             |
|-----------|--------------------------------------------------------------------------|
| rec count | cpu %           | mem % | cpu %           | mem % | cpu %        | mem % |
| 0         | 0               | 1     | 0               | 0     | 0            | 0     |
| 1         | 141             | 5     | 110             | 3     | 131          | 2     |
| 2         | 241             | 10    | 204             | 5     | 175          | 3     |
| 3         | 406             | 14    | 275             | 7     | 250          | 5     |
| 4         | 639             | 19    | 434             | 9     | 340          | 7     |
| 5         |                 |       | 606             | 11    | 433          | 8     |
| 6         |                 |       | 736             | 13    | 547          | 10    |
| 7         |                 |       |                 |       | 502          | 12    |
| 8         |                 |       |                 |       | 604          | 13    |
| 9         |                 |       |                 |       | 678          | 15    |
| 10        |                 |       |                 |       | 781          | 17    |
| 11        |                 |       |                 |       | 913          | 18    |
| 12        |                 |       |                 |       | 1030         | 19    |
| 13        |                 |       |                 |       | 1260         | 21    |
| 14        |                 |       |                 |       | 1468         | 23    |
| 15        |                 |       |                 |       | 1563         | 25    |
```
