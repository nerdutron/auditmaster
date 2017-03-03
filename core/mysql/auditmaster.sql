CREATE TABLE operators (
	operator_id INT(11) auto_increment primary key NOT NULL,
	legal_name varchar(255),
	trading_name varchar(255),
	abn varchar(16),
	owners_name varchar(155),
	created_by int(11),
	email varchar(128),
	website varchar(128),
	phone varchar(32),
	fax varchar(32),
	last_audit_date date,
	address_1 varchar(128),
	address_2 varchar(128),
	suburb varchar(64),
	postcode varchar(4),
	state_id int(11),
	operator_status_id int(11));

CREATE TABLE operator_status (
	operator_status_id int(11) auto_increment primary key NOT NULL,
	status int(11));

CREATE TABLE contacts (
	contact_id int(11) auto_increment primary key NOT NULL,
	operator_id int(11) NOT NULL,
	surname varchar(64),
	firstname varchar(64),
	nickname varchar(64),
	phone varchar(32),
	fax varchar(32),
	mobile varchar(32),
	email varchar(64),
	position varchar(64));

CREATE TABLE notes (
	note_id int(11) auto_increment primary key NOT NULL,
	operator_id int(11) NOT NULL,
	note text);

CREATE TABLE suburbs (
	suburb_id int(11) auto_increment primary key NOT NULL,
	suburb varchar(64),
	state char(3),
	region int(11));

CREATE TABLE states (
	state_code varchar(3) NOT NULL,
	state varchar(16));

CREATE TABLE regions (
	region_id int(11) auto_increment primary key NOT NULL,
	region_name varchar(64),
	start_postcode char(4),
	end_postcode char(4));

CREATE TABLE audits (
	audit_id int(11) auto_increment primary key NOT NULL,
	operator_id int(11) NOT NULL,
	auditor_id int(11) NOT NULL,
	stage_id int(11) NOT NULL,
	scheme_id int(11) NOT NULL,
	cost decimal(16,2),
	paid decimal(16,2),
	date_due date,
	date_completed date,
	date_closed date,
	audit_status_id int(11));

CREATE TABLE audit_notes (
	audit_note_id int(11) auto_increment primary key NOT NULL,
	audit_id int(11) NOT NULL,
	note text);

CREATE TABLE audit_status (
	audit_status_id int(11) auto_increment primary key NOT NULL,
	audit_status varchar(32));

ALTER TABLE users ADD COLUMN auditor char(1);

CREATE TABLE schemes (
	scheme_id int(11) auto_increment primary key NOT NULL,
	scheme_name varchar(128));

CREATE TABLE scheme_signup (
	scheme_signup_id int(11) auto_increment primary key NOT NULL,
	operator_id int(11) NOT NULL,
	scheme_id int(11) NOT NULL,
	stage_id int(11) NOT NULL,
	start_date date,
	create_date date,
	signup_status int(11));

CREATE TABLE signup_status (
	signup_status_id int(11) auto_increment primary key NOT NULL,
	signup_status varchar(128));

CREATE TABLE stages (
	stage_id int(11) auto_increment primary key NOT NULL,
	description varchar(128),
	months int(5));


INSERT INTO stages VALUES (1, "Entry", 0);
INSERT INTO stages VALUES (2, "5 mth Compliance", 6);
INSERT INTO stages VALUES (3, "Annual Compliance", 12);
INSERT INTO stages VALUES (4, "18 mth Re-accreditation", 18);
INSERT INTO stages VALUES (5, "2 year Re-accreditation", 24);
	
INSERT INTO states VALUES ("QLD", "Queensland");
INSERT INTO states VALUES ("NSW", "New South Wales");
INSERT INTO states VALUES ("VIC", "Victoria");
INSERT INTO states VALUES ("NT", "Northern Territory");
INSERT INTO states VALUES ("SA", "South Australia");
INSERT INTO states VALUES ("TAS", "Tasmania");
INSERT INTO states VALUES ("WA", "Western Australia");

