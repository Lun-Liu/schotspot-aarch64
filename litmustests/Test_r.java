class Test_r {
	int x = 0;
	int y = 0;
	int tmp = 0;

	class Thread1 extends Thread {
		public void run(){
			x = 1;
			y = 1;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			y = 2;
			tmp = x;
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
			Test_r test_r = new Test_r();
			test_r.test();
			if(test_r.tmp == 0 && test_r.y == 2) 
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM R Test PASSED after 100000 iterations");
		else
			System.out.println("ARM R Test FAILED: problem occurs on iteration " + counter);
	}
}
