# Document editor

* User enters commands in a command line
* Commands allow to edit and print document on the screen
* Supported commands:
  
  - Exit - exits the app
    
  - Print - prints document on a screen

    ```
    > Enter a command:
    > Print
    > [text of document]
    ```
  - AddText
    - prompts the user to write a line that will be appended to a document

    ```
    > Enter a command:
    > AddText
    > Write text: line1   # line1 is appended to a document
    > Enter a command:
    > Print
    > [line1]
    ```

  - ToUpper 
    - changes letters to uppercase

    ```
    > Enter a command:
    > ToUpper
    > Enter a command:
    > Print
    > [LINE1]
    ```

  - ToLower
    - changes letters to lowercase

  - Copy
    - copies the whole content of document to a clipboard

  - Paste
    - appends content of a clipboard to a document

* Unknown command prints a message

  ```
  > Enter a command:
  > Cmd
  > Unknown command: Cmd
  > Enter a command:
  ```