#include"db.hpp"
#include"httplib.h"
using namespace MyBlog_System;
using namespace std;
#define WWWROOT "./www"

MyBlog_System::Table_User* table_user;
MyBlog_System::Table_Tag* table_tag;
MyBlog_System::Table_Blog* table_blog;


//table_user 相关回调函数
void InsertUser(const httplib::Request& req,httplib::Response& res){
  std::cout<<"添加一个新用户"<<std::endl;
  //首先需要获取其req对象中的正文信息
  std::string json_str = req.body;
  //将json_str转化为Json::Value对象 传递db.hpp中的接口
  Json::Reader reader;
  Json::Value value;
  bool ret = reader.parse(json_str,value);
  if(ret==false){
    printf("insert user infor pares json faile!\n");
    res.status = 400;//设置状态码（请求格式错误，服务器无法处理这个请求）
    return;
  }
  //访问数据库
  ret = table_user->Insert(value);
  if(ret==false){
    printf("Insert user info,dp error!\n");
    res.status = 500; //服务器内部错误
    return;
  }
  res.status = 200; //请求成功
  return;
}

void DeleteUser(const httplib::Request& req,httplib::Response& res){
  std::cout<<"删除一个用户"<<std::endl;
  //获取需要删除的用户ID
  int user_id = std::stoi(req.matches[1]);
  bool ret = table_user->Delete(user_id);
  if(ret==false){
    printf("Delete user info,dp error!\n");
    res.status = 500; //服务器内部错误
    return;
  }
  res.status = 200;
  return;
}

void UpdateUser(const httplib::Request& req,httplib::Response& res){ 
  std::cout<<"修改一个用户信息"<<std::endl;
  //获取需要修改用户的id
  int user_id = std::stoi(req.matches[1]);
  //将req.body转化为Json::Value对象
  std::string json_str = req.body;
  Json::Reader reader;
  Json::Value value;
  bool ret = reader.parse(json_str,value);
  if(ret == false){
    printf("Update User info parse fail\n");
    res.status = 400;
    return;
  }
  ret = table_user->Update(user_id,value); 
  if(ret==false){
    printf("Update user info,dp error!\n");
    res.status = 500;
    return;
  }
  res.status = 200;
  return;
}

void GetOneUser(const httplib::Request& req,httplib::Response& res){
  std::cout<<"查询一个用户"<<std::endl;
    //获取user_id
    int user_id = std::stoi(req.matches[1]);
    //定义一个Json::Value对象去获取该用户信息
    Json::Value value;
    bool ret = table_user->GetOne(user_id,&value);
    if(ret == false){
      printf("Get One user info,dp error!\n");
      res.status = 500;
      return;
    }
    //用Json::FastWriter对象，将value对象解析为json字符串，填充到res中
    Json::FastWriter writer;
    std::string json_str = writer.write(value);
    res.set_content(json_str,"application/json");
    res.status = 200;
    return;
}

void GetAllUser(const httplib::Request& req,httplib::Response& res){
  std::cout<<"无差别查询所有用户"<<std::endl;
  Json::Value value;
  bool ret = table_user->GetAll(&value);
  if(ret==false){
      printf("Get All user info,dp error!\n");
      res.status = 500;
      return;
  }
  //将Json::Value解析为string
  Json::FastWriter writer;
  std::string json_str = writer.write(value);
  res.set_content(json_str,"application/json");
  res.status = 200;
  return;
}

//关于table_tag的回调函数
void InsertTag(const httplib::Request& req,httplib::Response& res){
  std::cout<<"添加一个新标签"<<std::endl;
  //首先需要获取其req对象中的正文信息
  std::string json_str = req.body;
  //将json_str转化为Json::Value对象 传递db.hpp中的接口
  Json::Reader reader;
  Json::Value value;
  bool ret = reader.parse(json_str,value);
  if(ret==false){
    printf("insert tag infor pares json faile!\n");
    res.status = 400;//设置状态码（请求格式错误，服务器无法处理这个请求）
    return;
  }
  //访问数据库
  ret = table_tag->Insert(value);
  if(ret==false){
    printf("Insert tag info,dp error!\n");
    res.status = 500; //服务器内部错误
    return;
  }
  res.status = 200; //请求成功
  return;
}

void DeleteTag(const httplib::Request& req,httplib::Response& res){
  std::cout<<"删除一个标签"<<std::endl;
  //获取需要删除的标签ID
  int tag_id = std::stoi(req.matches[1]);
  bool ret = table_tag->Delete(tag_id);
  if(ret==false){
    printf("Delete tag info,dp error!\n");
    res.status = 500; //服务器内部错误
    return;
  }
  res.status = 200;
  return;
}

void UpdateTag(const httplib::Request& req,httplib::Response& res){
  std::cout<<"修改一个标签"<<std::endl;
  //获取需要修改标签的id
  int tag_id = std::stoi(req.matches[1]);
  //将req.body转化为Json::Value对象
  std::string json_str = req.body;
  Json::Reader reader;
  Json::Value value;
  bool ret = reader.parse(json_str,value);
  if(ret == false){
    printf("Update Tag info parse fail\n");
    res.status = 400;
    return;
  }
  ret = table_tag->Update(tag_id,value); 
  if(ret==false){
    printf("Update tag info,dp error!\n");
    res.status = 500;
    return;
  }
  res.status = 200;
  return;
}

void GetOneTag(const httplib::Request& req,httplib::Response& res){
  std::cout<<"查询一个标签"<<std::endl;
    //获取tag_id
    int tag_id = std::stoi(req.matches[1]);
    //定义一个Json::Value对象去获取该标签信息
    Json::Value value;
    bool ret = table_tag->GetOne(tag_id,&value);
    if(ret == false){
      printf("Get One tag info,dp error!\n");
      res.status = 500;
      return;
    }
    //用Json::FastWriter对象，将value对象解析为json字符串，填充到res中
    Json::FastWriter writer;
    std::string json_str = writer.write(value);
    res.set_content(json_str,"application/json");
    res.status = 200;
    return;
}

void GetAllTag(const httplib::Request& req,httplib::Response& res){
  std::cout<<"无差别查询所有标签"<<std::endl;
  Json::Value value;
  bool ret = table_tag->GetAll(&value);
  if(ret==false){
      printf("Get All tag info,dp error!\n");
      res.status = 500;
      return;
  }
  //将Json::Value解析为string
  Json::FastWriter writer;
  std::string json_str = writer.write(value);
  res.set_content(json_str,"application/json");
  res.status = 200;
  return;
}

//blog相关回调函数
void InsertBlog(const httplib::Request& req,httplib::Response& res){
  std::cout<<"添加一个新博客"<<std::endl;
  //首先需要获取其req对象中的正文信息
  std::string json_str = req.body;
  //将json_str转化为Json::Value对象 传递db.hpp中的接口
  Json::Reader reader;
  Json::Value value;
  bool ret = reader.parse(json_str,value);
  if(ret==false){
    printf("insert user infor pares json faile!\n");
    res.status = 400;//设置状态码（请求格式错误，服务器无法处理这个请求）
    return;
  }
  //访问数据库
  ret = table_blog->Insert(value);
  if(ret==false){
    printf("Insert blog info,dp error!\n");
    res.status = 500; //服务器内部错误
    return;
  }
  res.status = 200; //请求成功
  return;
}

void DeleteBlog(const httplib::Request& req,httplib::Response& res){
  std::cout<<"删除一个博客"<<std::endl;
  //获取需要删除的博客ID
  int blog_id = std::stoi(req.matches[1]);
  bool ret = table_blog->Delete(blog_id);
  if(ret==false){
    printf("Delete blog info,dp error!\n");
    res.status = 500; //服务器内部错误
    return;
  }
  res.status = 200;
  return;
}

void UpdateBlog(const httplib::Request& req,httplib::Response& res){
  std::cout<<"修改一个博客"<<std::endl;
  //获取需要修改blog的id
  int blog_id = std::stoi(req.matches[1]);
  //将req.body转化为Json::Value对象
  std::string json_str = req.body;
  Json::Reader reader;
  Json::Value value;
  bool ret = reader.parse(json_str,value);
  if(ret == false){
    printf("Update User info parse fail\n");
    res.status = 400;
    return;
  }
  ret = table_blog->Update(blog_id,value); 
  if(ret==false){
    printf("Update blog info,dp error!\n");
    res.status = 500;
    return;
  }
  res.status = 200;
  return;
}

void GetAllBlog(const httplib::Request& req,httplib::Response& res){
    Json::Value value;
   if(req.has_param("user_id")){
    std::cout<<"根据用户id查询博客"<<std::endl;
    int user_id = std::stoi(req.get_param_value("user_id"));
    bool ret = table_blog->GetUser(user_id,&value);
    if(ret==false){
      printf("get one_user blog info,dp error!\n");
      res.status = 500;
      return;
    }
  }
  else if(req.has_param("tag_id")){
    std::cout<<"根据标签id查询博客"<<std::endl;
    int tag_id = std::stoi(req.get_param_value("tag_id"));
    bool ret = table_blog->GetTag(tag_id,&value);
    if(ret==false){
      printf("get one_tag blog info,dp error!\n");
      res.status = 500;
      return;
    }
  }
  else{
    std::cout<<"无差别所有查询博客"<<std::endl;
    bool ret = table_blog->GetAll(&value);
    if(ret==false){
      printf("get all blog info,dp error!\n");
      res.status = 500;
      return;
    }
  }
    //填充将res的正文信息
    Json::FastWriter writer;
    std::string json_str = writer.write(value);
    res.set_content(json_str,"application/json");
    res.status = 200;
    return;
}
void GetOneBlog(const httplib::Request& req,httplib::Response& res){
  std::cout<<"查询指定博客"<<std::endl;
  int blog_id = stoi(req.matches[1]);
  Json::Value value;
  bool ret = table_blog->GetBlog(blog_id,&value);
  if(ret==false){
      printf("Get One blog info,dp error!\n");
      res.status = 500;
      return;
  }
  //将Json::Value解析为string
  Json::FastWriter writer;
  std::string json_str = writer.write(value);
  res.set_content(json_str,"application/json");
  res.status = 200;
  return;
}


int main(){
  
  //创建MYSQL描述符并初始化
  MYSQL* mysql = MyBlog_System::MysqlInit();
  if(mysql==NULL){
    return -1;
  }
  
  //初始化user tag blog对象
  table_user = new Table_User(mysql);
  table_tag = new Table_Tag(mysql);
  table_blog = new Table_Blog(mysql);

  //创建httplib::Server对象
  httplib::Server server;

//httplib::Server::set_base_dir() 设置url中的资源路径的相对根目录。
//控制客户端不能访问到服务器路径,好处:一些相对根目录下的静态
//文件资源，httplib会直接自动进行读取响应。
 server.set_base_dir(WWWROOT);


  //增 删 改 查
  //R"()"---括号中的数据是原始数据，去除特殊字符的特殊含义 c++11
  //路由注册函数的第一个参数可以是一个正则表达式
  //(\d+) 匹配连续数字 并捕捉匹配到的数字
  server.Post(R"(/user)",InsertUser); 
  server.Delete(R"(/user/(\d+))",DeleteUser);
  server.Put(R"(/user/(\d+))",UpdateUser);
  server.Get(R"(/user)",GetAllUser);
  server.Get(R"(/user/(\d+))",GetOneUser);
 
  server.Post(R"(/tag)",InsertTag); 
  server.Delete(R"(/tag/(\d+))",DeleteTag);
  server.Put(R"(/tag/(\d+))",UpdateTag);
  server.Get(R"(/tag)",GetAllTag);
  server.Get(R"(/tag/(\d+))",GetOneTag);
   
  server.Post(R"(/blog)",InsertBlog); 
  server.Delete(R"(/blog/(\d+))",DeleteBlog);
  server.Put(R"(/blog/(\d+))",UpdateBlog);
  // get /blog 代表请求所有  get /blog？tag_id = 1 /blog?user_id=1 代表查询某一个
  server.Get(R"(/blog)",GetAllBlog); 
  server.Get(R"(/blog/(\d+))",GetOneBlog);


  //侦听连接
  server.listen("0.0.0.0",8080);

  return 0;
}

