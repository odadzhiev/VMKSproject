from http.server import HTTPServer, SimpleHTTPRequestHandler
import json

latest_data = {}

class Handler(SimpleHTTPRequestHandler):
    def do_POST(self):
        if self.path == '/data':
            length = int(self.headers['Content-Length'])
            body = self.rfile.read(length)
            global latest_data
            latest_data = json.loads(body)
            self.send_response(200)
            self.end_headers()

    def do_GET(self):
        if self.path == '/data':
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps(latest_data).encode())
        else:
            super().do_GET()

    def log_message(self, format, *args):
        pass  # изключва логовете в конзолата

HTTPServer(('0.0.0.0', 8080), Handler).serve_forever()