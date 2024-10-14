-- Initialize database
-- SHELL: cat create_tables.sql | sqlite3 user_account.sqlite

-- create tables
CREATE TABLE IF NOT EXISTS t_user_base_info (
   uid          INT PRIMARY KEY     NOT NULL,
   name         VARCHAR(100)        NOT NULL,
   age          INT,
   gender       INT,
   creation_time    INT,
   lang         VARCHAR(8),
   country      VARCHAR(8),
   address      VARCHAR(200)
);

-- insert records
INSERT INTO t_user_base_info VALUES 
(1, 'Yuu', 18, 1, 1728878691, 'zh-CN', 'China', 'ShenZhen'),
(2, 'Nobody', 28, 1, 1728878794, 'en-US', 'US', 'Los Angles');
