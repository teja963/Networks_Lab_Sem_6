<!DOCTYPE html>
<head>
  <title >Demo</title>
</head>

<body>
    <form id="form" action="#">
           <div class="inputbox" id="">
               <span style="color: rgb(71, 71, 71);">Email</span>
               <input type="text" name="Email_id" id="email" onkeydown="validation()"required/>            
               <span id="text"></span>
           </div>
    </form>
    
    <script type="text/javascript">
         function validation()
         {
            var form=document.getElementById("form");
            var email=document.getElementById("email").value;
            var text=document.getElementById("text");
            var pattern = /^[^ ]+@[^ ]+\.[a-z]{2,3}$/;
            
            if(email.match(pattern)) 
            {
               form.classList.add("valid");
               form.classList.remove("invalid");
               text.innerHTML="Your Email is Valid";
               text.style.color="#00ff00";
               
            }
            else
            {
               form.classList.remove("valid");
               form.classList.add("invalid"); 
               text.innerHTML="Please Enter Valid Email";
               text.style.color="#ff0000"; 
            }
            
            
         }
          document.querySelector('.img__btn').addEventListener('click', function() {
              document.querySelector('.cont').classList.toggle('s--signup');
          });
      </script>
    
</body>
</html>
