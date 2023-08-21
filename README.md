# Webserver using C

This is a simple web server using the C language. This handles the following functionalities.

- GET requests
- POST requests
- Multiple file types such as images, text files and PDF files

Other than the above this code will check if there are any parameters in the request URL (example - _http://127.0.0.1:8080/api/getNames?age=24_). If there are any parameters then it will remove them. Also, this will handle any invalid endpoint by serving a 404 error file.

To run this code just simply compile the code and execute it. By default, the server will open in port 8080 and you can change it if you want. Just change the below code to change the port number.

```
#define PORT 8080
```

Run the file using the command below.

```
if ($?) { gcc webserver.c -o webserver } ; if ($?) { .\webserver }
```

Note that if you are running this code in a Windows machine you will need the **Cygwin64** compiler.
