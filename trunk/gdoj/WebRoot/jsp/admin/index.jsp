<%@ page language="java" import="java.util.*" pageEncoding="UTF-8"%>
<%@taglib uri="/struts-tags" prefix="s"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">

<html>
  <head>
    <base href="<%=basePath%>">
    <title>Welcome to GuiLin University of Electronic Technology Online Judge System</title>
	<meta http-equiv="pragma" content="no-cache">
    <meta http-equiv="expires" content="-1">
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
	<meta http-equiv="keywords" content="GUET,ACM,OnlineJudge,JAVA,C++,Program Contest">
	<meta http-equiv="description" content="GuiLin University of Electronic Technology Online Judge System fof ACM">
 	<link href="css/styles.css" type="text/css" rel="stylesheet">
 	<link href="css/jquery-ui.css" rel="stylesheet" type="text/css"/>
    <style>
	.ui-autocomplete-loading { background: white url('img/ui-anim_basic_16x16.gif') right center no-repeat; }
	.ui-autocomplete {
		max-height: 200px;
		overflow-y: auto;
		/* prevent horizontal scrollbar */
		overflow-x: hidden;
		/* add padding to account for vertical scrollbar */
		padding-right: 3px;
		
		font-size:11px;
		line-height: 12px;
	}
	/* IE 6 doesn't support max-height
	 * we use height instead, but this forces the menu to always be this tall
	 */
	* html .ui-autocomplete {
		height: 200px;
	}
	
	</style>
    
	 
  </head>
  
  <body>
  <jsp:include  page="../head.jsp" ></jsp:include> 
 
  <div id="body">
   	<div id="content">
   		<div id="nav-content" >	
   		<a href="admin">Admin</a>
   		<a href="admin/news">News</a>
     	<a href="admin/problemset">Problems</a>
     	<a href="admin/problemset/status">Status</a>
     	<a href="admin/contests">Contests</a>
     	<a href="admin/user">Users</a>
     		<a href="admin/privilege">Privilege</a>	
    	 </div>
    	 <div>
    	 	<a href="admin/home-edit">编辑主页</a>(每小时自动生成)<br/>
			<a href="admin/recent-action">生成RecentAction页面</a>(每小时自动生成)<br/>
     		<a href="admin/topUsers">生成Top10页面</a>(每小时自动生成)<br/>
			<a href="admin/OpenPath.action?path=OJ_TMP">查看输出文件目录</a><br/>		
     	</div>		
   	</div>
    <jsp:include  page="/jsp/footer.jsp" ></jsp:include>
  </div>
  </body>
</html>
