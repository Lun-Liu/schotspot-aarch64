class Test_S {
	int x = 0;
	int y = 0;
	int tmp = 0;

	class Thread1 extends Thread {
		public void run(){
			x = 2;
			y = 1;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmp = y;
			x = 1;   
		}
	}    

	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();

		t1.start();
		t2.start();

		try {
			t1.join();
			t2.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			Test_S test_s = new Test_S();
			test_s.test();
			if(test_s.tmp == 1 && test_s.x == 2) 
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM S Test PASSED after 100000 iterations");
		else
			System.out.println("ARM S Test FAILED: problem occurs on iteration " + counter);
	}
}
