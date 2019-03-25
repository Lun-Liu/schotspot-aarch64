class Test_22W_Array {
	int[] x = {0,0};

	class Thread1 extends Thread {
		public void run(){
			x[0] = 2;
			x[1] = 1;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			x[1] = 2;
			x[0] = 1;
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
			Test_22W_Array test_22w = new Test_22W_Array();
			test_22w.test();
			if(test_22w.x[0] == 2 && test_22w.x[1] == 2) 
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM 2+2W Test PASSED after 100000 iterations");
		else
			System.out.println("ARM 2+2W Test FAILED: problem occurs on iteration " + counter);
	}
}
