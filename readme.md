## Example of event sourcing pattern

### Init database

create table Entiry (id int not null auto_increment,value varchar(256), version int, primary key (id));

create table Event (id int not null auto_increment,entity_id int, value_change varchar(256), version int, primary key (id));

### Change entity request


##
