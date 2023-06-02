class Seller:
    def __init__(self, seller_id="", session_count=0, plan_id=0):
        self.seller_id = seller_id
        self.session_count = session_count
        self.plan_id = plan_id

    def __str__(self):
        return self.seller_id
