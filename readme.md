## Example of event sourcing pattern

### Init database

create table Entity (id int not null,value varchar(256), version int, primary key (id));

create table Event (id int not null auto_increment,entity_id int, event_value varchar(256), primary key (id));

### Change entity request

http://192.168.1.84:8080/request/1001?value=test_value_100_3

### Get event log

http://192.168.1.84:8080/events?from=1
