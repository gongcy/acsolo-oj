<%@ page language="java" import="java.util.*" pageEncoding="UTF-8"%>
 <%@ taglib prefix="s" uri="/struts-tags" %>
 

 <div id="footer">
    Copyright &copy; 2011-2012 <a href="mailto:269574524@qq.com" style="text-decoration: none;">Zengke Wei</a>. All Rights Reserved. <a href="https://sourceforge.net/projects/gdoj/" style="text-decoration: none;" target="_blank">Open Source Project</a><br>
	The Programing Contest Web 1.5 platform & Judge Kernel 1.0 version.
</div>

<script type="text/javascript">
$(document).ready(function() {	
     OnlineJudge.saveUrl();
     
     scrolltotop.offset(100,120);
	 scrolltotop.init();
     /*
 * User Tip
 * 
 */
$(function(){	
	$(".user-tip").poshytip({	
		className: 'tip-yellowsimple',		
		alignTo: 'target',		
		alignX: 'right',
		alignY: 'bottom',
		offsetX: 5,
		offsetY: -25,			
		content: function(updateCallback) {
			
			$.post("userInfo.action", { username:$(this).attr('user')} ,function(data){
			//alert(data);
			updateCallback(data);	
			});
			
			/*alert($(this).attr('user'));
			ajax_get("userInfo?username="+$(this).attr('user'),true,function(html){	
			alert(html);
			updateCallback(html);			
			});
			*/	
			return "Loading...";		
		}	
	});	
});	
/*End User tip
*/
 });
</script>

