#include "http_server.hh"
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <typeinfo>
vector<string> split(const string &s, char delim) {
  vector<string> elems;

  stringstream ss(s);
  string item;

  while (getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }

  return elems;
}

HTTP_Request::HTTP_Request(string request) {
  vector<string> lines = split(request, '\n');
  vector<string> first_line = split(lines[0], ' ');
  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request
  /*
   TODO : extract the request method and URL from first_line here
  */
  this->method=first_line[0];
  this->url=first_line[1];
  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

HTTP_Response *handle_request(string req) {
HTTP_Request *request = new HTTP_Request(req);
 
  HTTP_Response *response = new HTTP_Response();
  string line;
  string url = string("html_files") + request->url;
  response->HTTP_version = "1.0";
  struct stat sb;
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";

    if (S_ISDIR(sb.st_mode)) {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */
     string dirurl=url+string("/index.html");
     ifstream file;
     string data;
     file.open(dirurl);
     if(stat(dirurl.c_str(),&sb)==0)
     {
         
         while(getline(file,line)){
          line=line+"\n";
          data.append(line);
         }
         response->body.append(data);
         stringstream ss;
         ss<<response->body.length();
         string s;
         ss>>s;
         response->content_length=s;
     }
 
    /*
    TODO : open the file and read its contents
    */

    /*
    TODO : set the remaining fields of response appropriately
    */
  }
  }

  else {
    response->status_code = "404";
    response->status_text="Not Found";
    response->content_type="text/html";
    string data="<!DOCTYPE html>\n<html>\n<head>\n<title>NotFound</title>\n</head>\n<body>\n<h1>Data Not Found</h1>\n</body>\n</html>\n";
    response->body.append(data);
    response->content_length=response->body.length();
    /*
    TODO : set the remaining fields of response appropriately
    */
  }

  delete request;

  return response;
}

string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */
string data;
data.append(string("HTTP/")+HTTP_version+string(" ")+status_code+string(" ")+status_text+string("\n")+string("Content-Type:")+content_type+string("\n")+string("Content-Length:")+content_length+string("\n\n")+body);
 
 return data;
}
int main()
{ string request="GET /apart1 HTTP/1.1";
  handle_request(request);
  

}
