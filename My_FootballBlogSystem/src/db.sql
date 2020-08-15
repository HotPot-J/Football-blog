drop database if exists MyBlog_System;
create database if not exists MyBlog_System;

use MyBlog_System;

create table if not exists table_user(
    id int auto_increment comment'用户id，自增长',
    user_name varchar(32) comment'用户昵称',
    primary key(id),
    unique key(user_name)
);

create table if not exists table_tag(
    id int auto_increment comment'标签id，自增长',
    tag_name varchar(64) comment'标签名',
    primary key(id)
);

create table if not exists table_blog(
  id int auto_increment comment'博客id，自增长',
  user_id int comment'外键，对应table_user中的id',
  tag_id int comment'外键，对应table_tag中的id',
  content text comment'正文',
  ctime datetime comment'博客最后改变时间',
  foreign key(user_id) references table_user(id),
  foreign key(tag_id) references table_tag(id),
  primary key(id)
);


