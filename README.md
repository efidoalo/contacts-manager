# contacts-manager
A program that provides easy interaction with a previously created PostgreSQL database to manage contacts.
The program allows for searching, deleting, and listing of contacts based on their first and/or last name(s) and inserting individuals in to the database with a (first name, last name, home phone number, mobile phone number, email addr, home addr, birthday) septuple of strings. Queries are made to a table named contact_data in a database named contacts.

The program was designed for my own personal use, but anybody may download, compile, link and run the contacts.c file. Possibly an installer will be included in the future that creates the database/table environment for the program to interact with, this would involve installing PostgreSQL and its C API library libpq.
