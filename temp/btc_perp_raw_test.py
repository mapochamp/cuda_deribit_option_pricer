import asyncio
import websockets
import json

# To subscribe to this channel:
msg = \
    {"jsonrpc": "2.0",
     "method": "public/subscribe",
     "id": 42,
     "params": {
         "channels": ["trades.option.BTC.100ms"]}
    }

async def call_api(msg):
    async with websockets.connect('wss://streams.deribit.com/ws/api/v2 ') as websocket:
       await websocket.send(msg)
       while websocket.open:
           response = await websocket.recv()
           # do something with the notifications...
           print("--------------------------------\n")
           print(response)
           print("--------------------------------\n")

asyncio.get_event_loop().run_until_complete(call_api(json.dumps(msg)))

