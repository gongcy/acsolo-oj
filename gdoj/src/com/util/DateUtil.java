package com.util;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class DateUtil {
	public static Date StringToDate(String dateString,String formatString){
		try {
			DateFormat fm = new SimpleDateFormat(formatString);
			Date date = null;
			try {
				date = fm.parse(dateString);
			} catch (java.text.ParseException e) {
				// TODO Auto-generated catch block
				//e.printStackTrace();
				return null;
			} // Thu Jan 18 00:00:00 CST 2007
			return date;
		} catch (Exception e) {
			// TODO: handle exception
			return null;
		}
	}
	public static String DateToString(Date date,String formatString){
		try {
			DateFormat fm = new SimpleDateFormat(formatString);
			String str = new String();
			str = fm.format(date);   
			return str;
		} catch (Exception e) {
			// TODO: handle exception
			return null;
		}
	}
	public static String penaltyString(int second) {
		
		int sec=second%60;
			second=second/60;
		int min=second%60;	
		int hour=second=second/60;
		
		String nowTime=new String();
		
		if(hour<10) {
			if(hour==0) nowTime="00";
			nowTime="0"+hour;
		}else {nowTime=Integer.toString(hour);}
	
		if(min<10) {
			nowTime=nowTime+":0"+min;
		}else {nowTime=nowTime+":"+min;}
		
		if(sec<10) {
			nowTime=nowTime+":0"+sec;
		}else {nowTime=nowTime+":"+sec;}
		
		return nowTime;
	}
	public static String secondToString(long second) {
		
		long sec=second%60;
			second=second/60;
		long min=second%60;	
		long hour=second=second/60;
		
		String nowTime=new String();
		
		if(hour<10) {
			if(hour==0) nowTime="00";
			nowTime="0"+hour;
		}else {nowTime=Long.toString(hour);}
	
		if(min<10) {
			nowTime=nowTime+":0"+min;
		}else {nowTime=nowTime+":"+min;}
		
		if(sec<10) {
			nowTime=nowTime+":0"+sec;
		}else {nowTime=nowTime+":"+sec;}
		
		return nowTime;
	}

}
