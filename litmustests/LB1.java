class LB1 {
	int x = 0;
	int y = 0;
	int tmpx = 0;
	int tmpy = 0;

	class Thread1 extends Thread {
		public void run(){
			tmpx = x;
			y = 1;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmpy = y;
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
			LB1 lb = new LB1();
			lb.test();
			if(lb.tmpy == 1 && lb.tmpx == 1)
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM LB Test PASSED after 100000 iterations");
		else
			System.out.println("ARM LB Test FAILED: problem occurs on iteration " + counter);
	}
}
