package com.util;

import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;

import com.gdoj.user.vo.User;

class ComparatorUser implements Comparator{

	 public int compare(Object arg0, Object arg1) {
	  String user0=(String)arg0;
	  String user1=(String)arg1;
	  return user0.compareTo(user1);
	 }
}

public class OnlineUserList {

	private static List<String> list = Collections.synchronizedList(new LinkedList<String>());
	
	public static boolean addUser(String username) {
		System.out.println(new Date()+":"+username+" Login");
		if(list.indexOf(username) == -1) {
			list.add(username);
			return true;
		}
		return false;
	}
	
	public static boolean removeUser(String username) {
		System.out.println(new Date()+":"+username+" Logout"); 
		if(list.remove(username)) {
			return true;
		}
		return false;
	}
	
	public static int userSize() {
		return list.size();
	}
	
	public static List<String> list() {
		ComparatorUser comparator=new ComparatorUser();
		Collections.sort(list, comparator);
		return list;
	}
}
