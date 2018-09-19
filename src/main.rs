extern crate mio;
extern crate http_muncher;
extern crate sha1;
extern crate rustc_serialize;

use mio::{Events, Poll, Ready, PollOpt, Token};
use mio::net::{TcpListener, TcpStream};
use std::net::{SocketAddr};
use std::io::{self, Read, Write};
use std::collections::HashMap;
use http_muncher::{Parser, ParserHandler};
use std::{str, fmt};
use rustc_serialize::base64::{ToBase64, STANDARD};
use sha1::{Sha1};


//https://habr.com/post/268609/
//https://carllerche.github.io/mio/mio/struct.Token.html

#[derive(PartialEq)]
enum ClientState {
    AwaitingHandshake,
    HandshakeResponse,
    Connected
}

struct WebSocketClient {
    socket: TcpStream,
    //token: Token,
    buf: Vec<u8>,
    state: ClientState
}

impl WebSocketClient {
    fn new(socket: TcpStream, token: Token) -> WebSocketClient {
        WebSocketClient {
            socket: socket,
            //token: token,
            buf: Vec::new(),
            state: ClientState::AwaitingHandshake,
        }
    }
}

impl fmt::Debug for WebSocketClient {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        fmt::Debug::fmt(&self.socket, f)
    }
}

fn gen_key(key: &String) -> String {
    let mut m = Sha1::new();

    m.update(key.as_bytes());
    m.update("258EAFA5-E914-47DA-95CA-C5AB0DC85B11".as_bytes());

    return m.digest().bytes().to_base64(STANDARD);
}

struct HttpParser {
    current_key: Option<String>,
    headers: HashMap<String, String>     
}

impl HttpParser {
    fn new() -> HttpParser {
        HttpParser {
            current_key: None,
            headers: HashMap::new()
        }
    }
}

impl ParserHandler for HttpParser { 
    fn on_header_field(&mut self, _parser: &mut Parser, header: &[u8]) -> bool {
       self.current_key = Some (str::from_utf8(header).unwrap().to_string()); 
       println!("{}: ", str::from_utf8(header).unwrap());
       true
   }

   fn on_header_value(&mut self, _parser: &mut Parser, value: &[u8]) -> bool {
       let str_value = str::from_utf8(value).unwrap().to_string(); 
       
       self.headers.insert(self.current_key.clone().unwrap(), str_value);

       println!("\t {}", str::from_utf8(value).unwrap().to_string());

       true
   }
}

fn main() {
    const SERVER: Token = Token(0);
    let mut next_socket_index = 0;

    let addr: SocketAddr = "127.0.0.1:8080".parse().unwrap();
    let server = TcpListener::bind(&addr).unwrap();

    let poll = Poll::new().unwrap();
    let mut events = Events::with_capacity(1024);


    poll.register(&server, SERVER, Ready::readable() | Ready::writable(), PollOpt::edge()).unwrap();

    let mut sockets = HashMap::new();

    let mut buf = [0; 32];

    let mut parser = Parser::request();

    loop {
        poll.poll(&mut events, None).unwrap();

        for event in events.iter() {
            println!("ready {:?} {:?}", event.token(), event.readiness());
            match event.token () {
                SERVER  => {
                    loop {
                        match server.accept() {
                            Ok((client_socket, client_addr)) => {
                                next_socket_index += 1;
                                let token = Token(next_socket_index);
        
                                poll.register(&client_socket, token, Ready::readable(), PollOpt::edge() | PollOpt::oneshot()).unwrap();

                                sockets.insert(token, WebSocketClient::new(client_socket, token));
        
                                println!("client socket {}", client_addr);
                            }
                            Err(ref e) if e.kind() == io::ErrorKind::WouldBlock => {
                                break;
                            }
                            e => panic!("err={:?}", e)
                        }                        
                    }        
                }
                token  => {
                    loop {
                        match sockets.get_mut(&token).unwrap().socket.read(&mut buf) {
                            Ok(0) => {
                                println!("close socket {:?}", sockets.get(&token));
                                sockets.remove(&token);
                                break;
                            }
                            Ok(cnt) => {
                                let mut client = sockets.get_mut(&token).unwrap();
                                let mut v: Vec<_> = buf.iter().cloned().collect();
                                client.buf.append(&mut v);

                                /*parser.parse(&mut HttpParser, &buf);



                                if parser.is_upgrade() {
                                    println!("is upgrade");
                                }*/



                                println!("token {:?} data len {} buf len {} cap {}", event.token(), cnt, client.buf.len(), client.buf.capacity());

                            },
                            Err(ref e) if e.kind() == io::ErrorKind::WouldBlock => {
                                let mut client = sockets.get_mut(&token).unwrap();

                                let mut http_parser = HttpParser::new();
                                parser.parse(&mut http_parser, client.buf.as_slice());

                                let ws_key = http_parser.headers.get("Sec-WebSocket-Key").unwrap();

                                let response_key = gen_key(&ws_key);

                                let response = fmt::format(format_args!("HTTP/1.1 101 Switching Protocols\r\n\
                                                 Connection: Upgrade\r\n\
                                                 Sec-WebSocket-Accept: {}\r\n\
                                                 Upgrade: websocket\r\n\r\n", response_key));

                                client.buf.clear();

                                client.socket.write(response.as_bytes()).unwrap();
                                client.state = ClientState::Connected;

                                //let mut client = sockets.get_mut(&token);        

                                println!("WouldBlock {:?}", &token);
                                break;
                            }
                            e => panic!("err={:?}", e)
                        }
                    }
                }
            }
        }
    }
}
