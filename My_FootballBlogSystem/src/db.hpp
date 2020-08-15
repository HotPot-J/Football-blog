#include<iostream>
#include<mysql/mysql.h>
#include<jsoncpp/json/json.h>
#include<mutex>
#include<cstdio>
#include<cstdlib>
#include<string>

#define MYSQL_HOST "127.0.0.1"
#define MYSQL_DB "MyBlog_System"
#define MYSQL_USER "root"
#define MYSQL_PASS "111111"

namespace MyBlog_System{
  static std::mutex mtx; //定义一个全局静态的互斥锁 用来保证访问数据的线程安全问题
    
    //初始化MySQL操作句柄
    static MYSQL* MysqlInit(){
        MYSQL* mysql= NULL; 
        mysql = mysql_init(NULL);//让该函数自动分配并初始化一个操作句柄并返回
        if(mysql==NULL){
          printf("mysql_init:%s\n",mysql_error(mysql));
          return NULL;
        }
        //初始化并成功返回
        //连接数据库
        if(mysql_real_connect(mysql,MYSQL_HOST,MYSQL_USER,MYSQL_PASS,MYSQL_DB,0,NULL,0)==NULL){
          //连接失败
          perror("mysql_real_connect\n");
          mysql_close(mysql);
          return NULL;
        }
      //设置字符集utf8
        if(mysql_set_character_set(mysql,"utf8")!=0){
          printf("mysql_set_character_set:%s\n",mysql_error(mysql));
          mysql_close(mysql);
          return NULL;
        }
        return mysql;
    }
 //关闭套接字描述符
    static void MysqlRelease(MYSQL* mysql){
      if(mysql!=NULL){
          mysql_close(mysql);
      }
    }
//对数据库进行访问操作
    static bool MysqlQuery(MYSQL* mysql,const std::string& str){
          int ret = mysql_query(mysql,str.c_str());
          if(ret!=0){
            printf("Query [%s] fail error:%s\n",str.c_str(),mysql_error(mysql)); 
            return false;
          }
          return true;
    }


//***具体实现对各个表的访问类

    //table_user
    class Table_User{
      public:
        Table_User(MYSQL* mysql)
          :_mysql(mysql)
        {}
      //插入一个用户信息 是一个JSON对象 user_name:name
        bool Insert(const Json::Value& user){
          #define ISERT_USER "insert into table_user values(null,'%s');"
          char tmp[4096] = {'0'};
          //value.asCString() 将value对应的key以string输出
          sprintf(tmp,ISERT_USER,user["user_name"].asCString());
          bool ret = MysqlQuery(_mysql,tmp);
          if(ret==true){
              return true;
          }
          return false;
        }
     //删除一个用户
        bool Delete(int user_id){
          #define DELETE_USER "delete from table_user where id<=>%d;"
          char tmp[4096] = {'0'};
          sprintf(tmp,DELETE_USER,user_id);
          bool ret = MysqlQuery(_mysql,tmp);
          if(ret==true){
            return true;
          }
          return false;
        }
    //查找一个用户
        bool GetOne(int user_id,Json::Value* user){
          #define GETONE_USER "select user_name from table_user where id<=>%d;"
          char tmp[4096] = {'0'};
          sprintf(tmp,GETONE_USER,user_id);
          bool ret = MysqlQuery(_mysql,tmp);
          if(ret==true){
              //查询命令执行成功则获取结果集
            std::unique_lock<std::mutex> lck(mtx);  
              MYSQL_RES* res = mysql_store_result(_mysql);
              int num_row = mysql_num_rows(res); 
              if(num_row!=1){
                perror("GetOne user mysql_num_rows()\n");
                mysql_free_result(res);
                return false;
              }
              //由于是单行数据 只需要找出这一行再逐个获取每一列即可
              MYSQL_ROW row = mysql_fetch_row(res);
              (*user)["id"] = user_id;
              (*user)["user_name"] = row[0];
              //释放结果集 避免内存泄漏
              mysql_free_result(res);
              return true;
          }
          return false;
        }
    //查找所有用户
        bool GetAll(Json::Value* users){
            #define GETALL_USER "select* from table_user;"
            char tmp[4096] = {'0'};
            sprintf(tmp,GETALL_USER);
            std::unique_lock<std::mutex> lck(mtx);  
            bool ret = MysqlQuery(_mysql,tmp);
            if(ret==true){
              MYSQL_RES* res = mysql_store_result(_mysql);
              int num_row = mysql_num_rows(res);
              if(num_row<1){
                //列数不正确
                perror("GetAll user mysql_num_rows()\n");
                mysql_free_result(res);
                return false;
              }
              //遍历结果集
              for(int i= 0;i<num_row;++i){
                  MYSQL_ROW row = mysql_fetch_row(res);//该函数获取完当前行 下一次会自动获取下一行
                  Json::Value user;
                  user["id"] = std::atoi(row[0]);
                  user["user_name"] = row[1];
                  users->append(user); //将Json对象添加到Json数组中去
              }
              mysql_free_result(res);
              return true;
            }
            return false;
        }
      //修改用户信息
        bool Update(int user_id,Json::Value& user){
              #define UPDATE_USER "update table_user set user_name ='%s'where id = %d;"
              char tmp[4096] = {'0'};
             sprintf(tmp,UPDATE_USER,user["user_name"].asCString(),user_id);   
             bool ret = MysqlQuery(_mysql,tmp);
             if(ret==true){
                return true;
             }
             return false;
        }   
      private:
        MYSQL* _mysql;
    };
  
    //table_tag
    class Table_Tag{
      public:
        Table_Tag(MYSQL* mysql)
          :_mysql(mysql)
        {}
      //插入一个标签信息 是一个JSON对象 user_name:name
        bool Insert(const Json::Value& tag){
          #define ISERT_TAG "insert into table_tag values(null,'%s');"
          char tmp[4096] = {'0'};
          //value.asCString() 将value对应的key以string输出
          sprintf(tmp,ISERT_TAG,tag["tag_name"].asCString());
          bool ret = MysqlQuery(_mysql,tmp);
          if(ret==true){
              return true;
          }
          return false;
        }
     //删除一个标签
        bool Delete(int tag_id){
          #define DELETE_TAG "delete from table_tag where id=%d;"
          char tmp[4096] = {'0'};
          sprintf(tmp,DELETE_TAG,tag_id);
          bool ret = MysqlQuery(_mysql,tmp);
          if(ret==true){
            return true;
          }
          return false;
        }
    //查找一个标签
        bool GetOne(int tag_id,Json::Value* tag){
          #define GETONE_TAG "select tag_name from table_tag where id=%d;"
          char tmp[4096] = {'0'};
          sprintf(tmp,GETONE_TAG,tag_id);
            std::unique_lock<std::mutex> lck(mtx);  
          bool ret = MysqlQuery(_mysql,tmp);
          if(ret==true){
              //查询命令执行成功则获取结果集
              MYSQL_RES* res = mysql_store_result(_mysql);
              int num_row = mysql_num_rows(res); 
              if(num_row!=1){
                perror("GetOne tag mysql_num_rows()\n");
                mysql_free_result(res);
                return false;
              }
              //由于是单行数据 只需要找出这一行再逐个获取每一列即可
              MYSQL_ROW row = mysql_fetch_row(res);
              (*tag)["id"] = tag_id;
              (*tag)["tag_name"] = row[0];
              //释放结果集 避免内存泄漏
              mysql_free_result(res);
              return true;
          }
          return false;
        }
    //查找所有标签
        bool GetAll(Json::Value* tags){
            #define GETALL_TAG "select* from table_tag;"
            std::unique_lock<std::mutex> lck(mtx);  
            char tmp[4096] = {'0'};
            sprintf(tmp,GETALL_TAG);
            bool ret = MysqlQuery(_mysql,tmp);
            if(ret==true){
              MYSQL_RES* res = mysql_store_result(_mysql);
              int num_row = mysql_num_rows(res);
              if(num_row<1){
                //列数不正确
                perror("GetAll tag mysql_num_rows()\n");
                mysql_free_result(res);
                return false;
              }
              //遍历结果集
              for(int i= 0;i<num_row;++i){
                  MYSQL_ROW row = mysql_fetch_row(res);//该函数获取完当前行 下一次会自动获取下一行
                  Json::Value tag;
                  tag["id"] = std::atoi(row[0]);
                  tag["tag_name"] = row[1];
                  tags->append(tag); //将Json对象添加到Json数组中去
              }
              mysql_free_result(res);
              return true;
            }
            return false;
        }
      //修改标签信息
        bool Update(int tag_id,Json::Value& tag){
              #define UPDATE_TAG "update table_tag set tag_name ='%s'where id = %d;"
              char tmp[4096] = {'0'};
             sprintf(tmp,UPDATE_TAG,tag["tag_name"].asCString(),tag_id);   
             bool ret = MysqlQuery(_mysql,tmp);
             if(ret==true){
                return true;
             }
             return false;
        }   
      private:
        MYSQL* _mysql;
    };
  
   //table_blog
    class Table_Blog{
      public:
        Table_Blog(MYSQL* mysql)
              :_mysql(mysql)
          {}
      //插入一条博客
          bool Insert(const Json::Value& blog){
              #define INSERT_BLOG "insert into table_blog values(null,%d,%d,'%s','%s',now());"
              char tmp[4096] = {'0'}; 
       sprintf(tmp,INSERT_BLOG,blog["user_id"].asInt(),blog["tag_id"].asInt(),blog["title"].asCString(),blog["content"].asCString());
              bool ret = MysqlQuery(_mysql,tmp);
              if(ret==true){
                return true;
              }
              return false;
          }
      //删除一条博客
          bool Delete(int blog_id){
            #define DELETE_BLOG "delete from table_blog where id=%d;"
            char tmp[4096] = {'0'};
            sprintf(tmp,DELETE_BLOG,blog_id);
            bool ret = MysqlQuery(_mysql,tmp);
            if(ret==true){
              return true;
            }
            return false;
          }
      
      //根据user_id查询博客
          bool GetUser(int user_id,Json::Value* blogs){
              #define GETUSER "select* from table_blog where user_id=%d;"
            std::unique_lock<std::mutex> lck(mtx);  
              char tmp[4096] = {'0'};
              sprintf(tmp,GETUSER,user_id);
              bool ret = MysqlQuery(_mysql,tmp);
              if(ret==true){
                  //获取结果集
                  MYSQL_RES* res = mysql_store_result(_mysql);
                  int num_row = mysql_num_rows(res);
                  if(num_row<=0){
                    printf("GetUserBlog mysql_num_rows fail\n");
                    mysql_free_result(res);
                    return false;
                  }
                  for(int i = 0;i<num_row;++i){
                    MYSQL_ROW row = mysql_fetch_row(res);
                    Json::Value blog;
                    blog["id"] =  std::stoi(row[0]);
                    blog["user_id"] = std::stoi(row[1]);
                    blog["tag_id"] = std::stoi(row[2]);
                    blog["title"] = row[3];
                    blog["content"] = row[4];
                    blog["ctime"] = row[5];
                    blogs->append(blog);
                  }
                  mysql_free_result(res);
                  return true;

              }
              return false;
          }

      //根据tag_id查询博客
          bool GetTag(int tag_id,Json::Value* blogs){
              #define GETTAG "select* from table_blog where tag_id=%d;"
            std::unique_lock<std::mutex> lck(mtx);  
              char tmp[4096] = {'0'};
              sprintf(tmp,GETTAG,tag_id);
              bool ret = MysqlQuery(_mysql,tmp);
              if(ret==true){
                  //获取结果集
                  MYSQL_RES* res = mysql_store_result(_mysql);
                  int num_row = mysql_num_rows(res);
                  if(num_row<=0){
                    printf("GetTagBlog mysql_num_rows fail\n");
                    mysql_free_result(res);
                    return false;
                  }
                  for(int i = 0;i<num_row;++i){
                    MYSQL_ROW row = mysql_fetch_row(res);
                    Json::Value blog;
                    blog["id"] =  std::stoi(row[0]);
                    blog["user_id"] = std::stoi(row[1]);
                    blog["tag_id"] = std::stoi(row[2]);
                    blog["title"] = row[3];
                    blog["content"] = row[4];
                    blog["ctime"] = row[5];
                    blogs->append(blog);
                  }
                  mysql_free_result(res);
                  return true;
              }
              return false;
          }

      //根据blog_id查询博客
          bool GetBlog(int blog_id,Json::Value* blog){
              #define GETBLOG "select* from table_blog where id=%d;"
            std::unique_lock<std::mutex> lck(mtx);  
              char tmp[4096] = {'0'};
              sprintf(tmp,GETBLOG,blog_id);
              bool ret = MysqlQuery(_mysql,tmp);
              if(ret==true){
                  //获取结果集
                  MYSQL_RES* res = mysql_store_result(_mysql);
                  int num_row = mysql_num_rows(res);
                  if(num_row!=1){
                    printf("GetBlog mysql_num_rows fail\n");
                    mysql_free_result(res);
                    return false;
                  }
                  MYSQL_ROW row = mysql_fetch_row(res);
                  (*blog)["id"] =  std::stoi(row[0]);
                  (*blog)["user_id"] = std::stoi(row[1]);
                  (*blog)["tag_id"] = std::stoi(row[2]);
                  (*blog)["title"] = row[3];
                  (*blog)["content"] = row[4];
                  (*blog)["ctime"] = row[5];
                  mysql_free_result(res);
                  return true;
              }
              return false;
          }

      //查询所有博客
          bool GetAll(Json::Value* blogs){
              #define GETALL "select* from table_blog;"
            std::unique_lock<std::mutex> lck(mtx);  
            bool ret = MysqlQuery(_mysql,GETALL);
              if(ret==true){
                  //获取结果集
                  MYSQL_RES* res = mysql_store_result(_mysql);
                  int num_row = mysql_num_rows(res);
                  if(num_row<1){
                    printf("GetAllBlog mysql_num_rows fail\n");
                    mysql_free_result(res);
                    return false;
                  }
                  for(int i = 0;i<num_row;++i){
                    MYSQL_ROW row = mysql_fetch_row(res);
                    Json::Value blog;
                    blog["id"] =  std::stoi(row[0]);
                    blog["user_id"] = std::stoi(row[1]);
                    blog["tag_id"] = std::stoi(row[2]);
                    blog["title"] = row[3];
                    blog["content"] = row[4];
                    blog["ctime"] = row[5];
                    blogs->append(blog);
                  }
                  mysql_free_result(res);
                  return true;
              }
              return false;
          }
          
       //修改博客
          bool Update(int blog_id,Json::Value& blog){
              #define UPDATE_BLOG "update table_blog set tag_id=%d,title='%s',content='%s' where id=%d;"
              char tmp[4096]={'0'};
              sprintf(tmp,UPDATE_BLOG,blog["tag_id"].asInt(),blog["title"].asCString(),blog["content"].asCString(),blog_id);
              bool ret = MysqlQuery(_mysql,tmp);
              if(ret==true){
                return true;
              }
              return false;
          }
          
      private:
        MYSQL* _mysql;
    };
}
