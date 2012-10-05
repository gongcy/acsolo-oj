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
    <title>Statistic Chart - GUET Online Judge</title>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
	<meta http-equiv="keywords" content="GUET,ACM,OJ,OnlineJudge,JAVA,C++,Program Contest">
	<meta http-equiv="description" content="GuiLin University of Electronic Technology Online Judge System for ACM">
	<link href="css/styles.css" type="text/css" rel="stylesheet">
  	<script type="text/javascript" src="js/jquery-1.7.1.js"></script>
  	<script type="text/javascript" src="js/gdoj.js"></script>
 <script type="text/javascript" src="js/highcharts/highcharts.js"></script>
<script type="text/javascript" src="js/highcharts/modules/exporting.js"></script>
  </head>

  <body>  
  	<jsp:include   page="/jsp/head.jsp"></jsp:include> 
  <div id="body"> 
     <div id="content" class=""> 	 <!-- class="content-with-sidebar" -->
   
	    <div class="content" style="border-left:4px solid #B9B9B9;padding:3px 3px 3px 0px;">
	    <center><h5><a href="problemset/problem/<s:property value="problemId"/>" style="color:#000;text-decoration: none;">
	    <s:property value="problemId"/>.<s:property value="problemTitle"/></a>
	    </h5></center>    	
		<script type="text/javascript">
				

		$(document).ready(function() {

		var series = {
			 data: []
		};

 		$.post(
                "chart",
                {type:"monthly"},
                function(data) {    
 
                   if (data.success != true) {
                    alert(data["error"]);                
                    return;
                  }	
                  	var options = {
					    chart: {
					        renderTo: 'container',
					       defaultSeriesType: 'line'
					    },
					    title: {
					        text: 'Monthly Submissions'
					    },
					    xAxis: {
							type: 'datetime',
							dataTimeLabelFormats: {
								month: '%e. %b',
								year: '%b'
							},
					        categories: []
					    },
					    yAxis: {
					        title: {
					            text: 'submissions'
					        },
					        min:0
					    },
					    tooltip: {
							crosshairs: true,
							shared: true
						},
					    plotOptions: {
							spline: {
								marker: {
									radius: 4,
									lineColor: '#666666',
									lineWidth: 1
								}
							}
						},
					    series: [{
						    name:"sub",
						    data:data.list
					    }]
					}
				   	var chart = new Highcharts.Chart(options);
                },
                "json"
        );
        
		
	
});
		

		</script>

       		<div id="container" style="width: 720px; height: 400px; margin: 0 auto"></div>
	   </div>    
	</div>   
    <jsp:include  page="/jsp/footer.jsp" ></jsp:include>
  </div>
  </body>
</html>
