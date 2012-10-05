<%@ page language="java" import="java.util.*" pageEncoding="UTF-8" errorPage="error.jsp"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
String basePath2 = request.getScheme()+"://"+request.getServerName()+path+"/";

%>
<%
	String url = request.getParameter("url");
	//System.out.println(path+basePath+"enter"+"---"+url+"+++"+session.getAttribute("session_url"));
	if(!url.equals(basePath2+"passwordRecovery")&&!url.equals(basePath2+"password-recovery")&&!url.equals(basePath2+"registration")&&!url.equals(basePath2+"register")&&!url.equals(basePath2+"login")&&!url.equals(basePath2+"enter")&&!url.equals(basePath2+"logout")){
		session.setAttribute("session_url", url);
	}
	//System.out.println(basePath2+" +++"+url+"+++"+session.getAttribute("session_url"));
 %>
